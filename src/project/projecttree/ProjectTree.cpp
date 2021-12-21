#include "ProjectTree.hpp"

#include <utils/algorithm.h>
#include <utils/utilsicons.h>

#include <set>

namespace XMakeProjectManager::Internal {
    static Q_LOGGING_CATEGORY(xmake_project_tree_log, "qtc.xmake.projecttree", QtDebugMsg);

    auto fromXMakeKind(Target::Kind kind) -> ProjectExplorer::ProductType {
        switch (kind) {
            case Target::Kind::BINARY: return ProjectExplorer::ProductType::App;
            case Target::Kind::SHARED:
            case Target::Kind::STATIC: return ProjectExplorer::ProductType::Lib;
        }

        return ProjectExplorer::ProductType::Other;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto buildTargetSourceTree(ProjectExplorer::VirtualFolderNode *node, const Target &target)
        -> void {
        for (const auto &group : target.sources) {
            for (const auto &file : group.sources) {
                qCDebug(xmake_project_tree_log) << "Source node " << file;
                node->addNestedNode(
                    std::make_unique<ProjectExplorer::FileNode>(Utils::FilePath::fromString(file),
                                                                ProjectExplorer::FileType::Source));
            }
        }
    }

    auto buildTargetHeaderTree(ProjectExplorer::VirtualFolderNode *node, const Target &target)
        -> void {
        for (const auto &file : target.headers) {
            qCDebug(xmake_project_tree_log) << "Header node " << file;
            node->addNestedNode(
                std::make_unique<ProjectExplorer::FileNode>(Utils::FilePath::fromString(file),
                                                            ProjectExplorer::FileType::Header));
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto addTargetNode(std::unique_ptr<XMakeProjectNode> &root, const Target &target)
        -> XMakeTargetNode * {
        using XMakeTargetNodePtr = XMakeTargetNode *;
        auto output_node         = XMakeTargetNodePtr { nullptr };

        root->findNode(
            [&output_node, &root, &target, path = Utils::FilePath::fromString(target.defined_in)](
                ProjectExplorer::Node *node) {
                if (node->filePath() == path.absolutePath()) {
                    auto *as_folder = dynamic_cast<ProjectExplorer::FolderNode *>(node);
                    if (as_folder) {
                        auto target_node =
                            std::make_unique<XMakeTargetNode>(path.absolutePath().pathAppended(
                                                                  target.name),
                                                              target.name,
                                                              fromXMakeKind(target.kind));
                        qCDebug(xmake_project_tree_log) << "Target node " << target_node->path();
                        target_node->setDisplayName(target.name);

                        output_node = target_node.get();

                        as_folder->addNode(std::move(target_node));
                    }

                    return true;
                }

                return false;
            });

        return output_node;
    }

    auto findCommonPath(const QStringList &list) {
        if (std::empty(list)) return QString {};

        auto dir_list = QStringList {};
        dir_list.reserve(std::size(list));
        for (const auto &item : list) {
            dir_list.emplace_back(Utils::FilePath::fromString(item).parentDir().path());
        }

        auto root = dir_list.front();

        for (const auto &item : dir_list) {
            if (root.length() > item.length()) root.truncate(item.length());

            for (auto i = 0; i < root.length(); ++i) {
                if (root[i] != item[i]) {
                    root.truncate(i);
                    break;
                }
            }
        }

        qCDebug(xmake_project_tree_log) << "common path for " << dir_list << " is " << root;

        return root;
    }

    auto findCommonDir(const QStringList &list) {
        if (std::empty(list)) return QString {};

        auto root = list.front();

        for (const auto &item : list) {
            if (root.length() > item.length()) root.truncate(item.length());

            for (auto i = 0; i < root.length(); ++i) {
                if (root[i] != item[i]) {
                    root.truncate(i);
                    break;
                }
            }
        }

        qCDebug(xmake_project_tree_log) << "common dir for " << list << " is " << root;

        return root;
    }

    auto findCommonPath(const Target::SourceGroupList &sources_list) -> QString {
        auto list = QStringList {};
        list.reserve(std::size(sources_list));

        for (const auto &sources : sources_list) {
            list.emplace_back(findCommonPath(sources.sources));
        }

        return findCommonDir(list);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ProjectTree::ProjectTree() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ProjectTree::buildTree(const Utils::FilePath &src_dir,
                                const TargetsList &targets,
                                const std::vector<Utils::FilePath> &bs_files)
        -> std::unique_ptr<XMakeProjectNode> {
        auto target_paths = std::set<Utils::FilePath> {};

        auto root = std::make_unique<XMakeProjectNode>(src_dir);

        qCDebug(xmake_project_tree_log) << targets.size() << "target(s) found";
        for (const auto &target : targets) {
            auto *parent = addTargetNode(root, target);

            auto common_path = findCommonPath(target.sources);
            auto node        = std::unique_ptr<ProjectExplorer::VirtualFolderNode> {};
            if (common_path.size() > 0) {
                auto node = std::make_unique<ProjectExplorer::VirtualFolderNode>(
                    Utils::FilePath::fromString(common_path));
                qCDebug(xmake_project_tree_log) << "Virtual sources node " << node->path();
                node->setPriority(ProjectExplorer::Node::DefaultFolderPriority + 5);
                node->setDisplayName("Source Files");
                node->setIsSourcesOrHeaders(true);
                node->setListInProject(false);
                node->setIcon(
                    [] { return QIcon::fromTheme("edit-copy", ::Utils::Icons::COPY.icon()); });

                buildTargetSourceTree(node.get(), target);

                parent->addNode(std::move(node));
            }

            common_path = findCommonPath(target.headers);

            if (common_path.size() > 0) {
                node = std::make_unique<ProjectExplorer::VirtualFolderNode>(
                    Utils::FilePath::fromString(common_path));
                qCDebug(xmake_project_tree_log) << "Virtual headers node " << node->path();
                node->setPriority(ProjectExplorer::Node::DefaultFolderPriority + 5);
                node->setDisplayName("Header Files");
                node->setIsSourcesOrHeaders(true);
                node->setListInProject(false);
                node->setIcon(
                    [] { return QIcon::fromTheme("edit-copy", ::Utils::Icons::COPY.icon()); });

                buildTargetHeaderTree(node.get(), target);

                parent->addNode(std::move(node));
            }

            target_paths.insert(Utils::FilePath::fromString(target.defined_in).absolutePath());
        }

        for (auto bs_file : bs_files) {
            if (!bs_file.toFileInfo().isAbsolute())
                bs_file = src_dir.pathAppended(bs_file.toString());

            root->addNestedNode(
                std::make_unique<ProjectExplorer::FileNode>(bs_file,
                                                            ProjectExplorer::FileType::Project));
        }

        return root;
    }
} // namespace XMakeProjectManager::Internal

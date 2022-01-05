#include "ProjectTree.hpp"

#include <utils/algorithm.h>
#include <utils/utilsicons.h>

#include <set>

#include <QLoggingCategory>

namespace XMakeProjectManager::Internal {
    static Q_LOGGING_CATEGORY(xmake_project_tree_log, "qtc.xmake.projecttree", QtDebugMsg);

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
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
    auto buildTargetSourceTree(ProjectExplorer::VirtualFolderNode *node,
                               const Target::SourceGroupList &sources) -> void {
        for (const auto &group : sources) {
            for (const auto &filename : group.sources) {
                auto file = Utils::FilePath::fromString(filename).absoluteFilePath();

                qCDebug(xmake_project_tree_log) << "Source node" << file.toUserOutput();
                node->addNestedNode(
                    std::make_unique<ProjectExplorer::FileNode>(file,
                                                                ProjectExplorer::FileType::Source),
                    {},
                    [](const Utils::FilePath &fn) {
                        qCDebug(xmake_project_tree_log) << "Folder node" << fn;
                        return std::make_unique<ProjectExplorer::FolderNode>(fn);
                    });
            }
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto buildTargetModuleTree(ProjectExplorer::VirtualFolderNode *node,
                               const Target::SourceGroupList &sources) -> void {
        auto nodes = std::vector<std::unique_ptr<ProjectExplorer::FileNode>> {};
        for (const auto &group : sources) {
            for (const auto &filename : group.sources) {
                auto file = Utils::FilePath::fromString(filename).absoluteFilePath();

                qCDebug(xmake_project_tree_log) << "Module node" << filename;
                nodes.emplace_back(
                    std::make_unique<ProjectExplorer::FileNode>(file,
                                                                ProjectExplorer::FileType::Header));
            }
        }

        node->addNestedNodes(std::move(nodes));

        for (auto *n : node->folderNodes()) n->compress();
    }

    auto buildTargetHeaderTree(ProjectExplorer::VirtualFolderNode *node, const QStringList &headers)
        -> void {
        for (const auto &filename : headers) {
            auto file = Utils::FilePath::fromString(filename).absoluteFilePath();

            qCDebug(xmake_project_tree_log) << "Header node" << file.toUserOutput();
            node->addNestedNode(
                std::make_unique<ProjectExplorer::FileNode>(file,
                                                            ProjectExplorer::FileType::Header),
                {},
                [](const Utils::FilePath &fn) {
                    qCDebug(xmake_project_tree_log) << "Folder node" << fn;
                    return std::make_unique<ProjectExplorer::FolderNode>(fn);
                });
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto addTargetNode(std::unique_ptr<XMakeProjectNode> &root, const Target &target)
        -> XMakeTargetNode * {
        using XMakeTargetNodePtr = XMakeTargetNode *;
        auto *output_node        = XMakeTargetNodePtr { nullptr };

        auto target_node =
            std::make_unique<XMakeTargetNode>(Utils::FilePath::fromString(target.defined_in)
                                                  .absolutePath(),
                                              target.name,
                                              fromXMakeKind(target.kind));
        qCDebug(xmake_project_tree_log)
            << "Target node " << target_node->path().toUserOutput() << " defined in"
            << Utils::FilePath::fromString(target.defined_in).toUserOutput();
        target_node->setDisplayName(target.name);

        output_node = target_node.get();

        root->addNode(std::move(target_node));

        return output_node;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto createVirtualNode(const Utils::FilePath &path, const QString &name)
        -> std::unique_ptr<ProjectExplorer::VirtualFolderNode> {
        if (path.isEmpty()) return nullptr;

        auto node = std::make_unique<ProjectExplorer::VirtualFolderNode>(path);

        qCDebug(xmake_project_tree_log)
            << QString { "Virtual node '%1' %2" }.arg(name, node->path().toUserOutput());
        node->setPriority(ProjectExplorer::Node::DefaultFolderPriority + 5);
        node->setDisplayName(name);
        node->setIsSourcesOrHeaders(true);
        node->setListInProject(false);
        node->setIcon([] { return QIcon::fromTheme("edit-copy", ::Utils::Icons::COPY.icon()); });

        return node;
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
            auto sources = target.sources;

            auto *parent = addTargetNode(root, target);

            auto modules_it =
                std::find_if(std::cbegin(sources), std::cend(sources), [](const auto &batch) {
                    return batch.language == "cxxmodule";
                });

            auto modules = Target::SourceGroupList {};
            if (modules_it != std::cend(sources)) {
                modules.emplace_back(*modules_it);

                sources.erase(modules_it);
            }

            auto base_directory = Utils::FilePath {};
            for (const auto &source_group : sources) {
                for (const auto &source : source_group.sources) {
                    const auto source_path = Utils::FilePath::fromString(source).cleanPath();

                    if (base_directory.isEmpty()) base_directory = source_path.parentDir();
                    else
                        base_directory = Utils::FileUtils::commonPath(base_directory, source_path);
                }
            }

            auto node = createVirtualNode(base_directory, "Source Files");
            if (node) {
                buildTargetSourceTree(node.get(), sources);
                parent->addNode(std::move(node));
            }

            if (!std::empty(modules)) {
                base_directory = Utils::FilePath {};
                for (const auto &source_group : modules) {
                    for (const auto &source : source_group.sources) {
                        const auto source_path = Utils::FilePath::fromString(source).cleanPath();

                        if (base_directory.isEmpty()) base_directory = source_path.parentDir();
                        else
                            base_directory =
                                Utils::FileUtils::commonPath(base_directory, source_path);
                    }
                }

                node = createVirtualNode(base_directory, "Module Files");
                if (node) {
                    buildTargetModuleTree(node.get(), modules);
                    parent->addNode(std::move(node));
                }
            }

            base_directory = Utils::FilePath {};
            for (const auto &source : target.headers) {
                const auto source_path = Utils::FilePath::fromString(source).cleanPath();

                if (base_directory.isEmpty()) base_directory = source_path.parentDir();
                else
                    base_directory = Utils::FileUtils::commonPath(base_directory, source_path);
            }

            node = createVirtualNode(base_directory, "Header Files");
            if (node) {
                buildTargetHeaderTree(node.get(), target.headers);
                parent->addNode(std::move(node));
            }

            target_paths.insert(Utils::FilePath::fromString(target.defined_in).absolutePath());
        }

        for (auto bs_file : bs_files) {
            if (!bs_file.toFileInfo().isAbsolute())
                bs_file = src_dir.pathAppended(bs_file.toString());

            root->findNode([&root, &bs_file](ProjectExplorer::Node *node) {
                qCDebug(xmake_project_tree_log) << node->filePath();
                if (node->filePath() == bs_file.absolutePath()) {
                    auto *as_folder = dynamic_cast<ProjectExplorer::FolderNode *>(node);
                    if (as_folder != nullptr) {
                        auto project_file_node = std::make_unique<ProjectExplorer::FileNode>(
                            bs_file.absoluteFilePath(),
                            ProjectExplorer::FileType::Project);
                        qCDebug(xmake_project_tree_log)
                            << "Project file node " << bs_file.toUserOutput();

                        as_folder->addNode(std::move(project_file_node));
                    }

                    return true;
                }

                return false;
            });
        }

        return root;
    }
} // namespace XMakeProjectManager::Internal

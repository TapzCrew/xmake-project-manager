#include "ProjectTree.hpp"

#include <set>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto buildTargetTree(std::unique_ptr<XMakeProjectNode> &root, const Target &target) -> void {
        const auto path = Utils::FilePath::fromString(target.defined_in);

        // TODO
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto addTargetNode(std::unique_ptr<XMakeProjectNode> &root, const Target &target) -> void {
        root->findNode([&root, &target, path = Utils::FilePath::fromString(target.defined_in)](
                           ProjectExplorer::Node *node) {
            if (node->filePath() == path.absolutePath()) {
                auto as_folder = dynamic_cast<ProjectExplorer::FolderNode *>(node);
                if (as_folder) {
                    auto target_node =
                        std::make_unique<XMakeTargetNode>(path.absolutePath().pathAppended(
                                                              target.name),
                                                          target.name);
                    target_node->setDisplayName(target.name);

                    as_folder->addNode(std::move(target_node));
                }

                return true;
            }

            return false;
        });
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

        for (const auto &target : targets) {
            buildTargetTree(root, target);

            // target_paths.insert(Utils::FilePath::fromString(target))

            addTargetNode(root, target);
        }

        for (auto bs_file : bs_files) {
            if (bs_file.toFileInfo().isAbsolute())
                bs_file = src_dir.pathAppended(bs_file.toString());

            root->addNestedNode(
                std::make_unique<ProjectExplorer::FileNode>(bs_file,
                                                            ProjectExplorer::FileType::Project));
        }

        return root;
    }
} // namespace XMakeProjectManager::Internal

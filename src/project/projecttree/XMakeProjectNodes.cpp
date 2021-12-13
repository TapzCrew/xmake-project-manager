#include <project/projecttree/XMakeProjectNodes.hpp>

#include <XMakeProjectConstant.hpp>
#include <project/XMakeBuildConfiguration.hpp>
#include <project/XMakeBuildSystem.hpp>

#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProjectNode::XMakeProjectNode(const Utils::FilePath &directory)
        : ProjectExplorer::ProjectNode { directory } {
        setPriority(Node::DefaultProjectPriority + 1000);
        setIcon(ProjectExplorer::DirectoryIcon { ProjectExplorer::Constants::FILEOVERLAY_PRODUCT });
        setListInProject(false);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeTargetNode::XMakeTargetNode(const Utils::FilePath &directory, const QString &name)
        : ProjectExplorer::ProjectNode { directory }, m_name { name } {
        setPriority(Node::DefaultProjectPriority + 900);
        setIcon(":/projectexplorer/images/build.png");
        setListInProject(false);
        setShowWhenEmpty(true);
        setProductType(ProjectExplorer::ProductType::Other);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTargetNode::build() -> void {
        auto project = getProject();
        auto target  = project ? project->activeTarget() : nullptr;

        // TODO
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTargetNode::tooltip() const -> QString { return {}; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTargetNode::buildKey() const -> QString { return m_name; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeFileNode::XMakeFileNode(const Utils::FilePath &file)
        : ProjectExplorer::ProjectNode { file } {
        setIcon(ProjectExplorer::DirectoryIcon {
            QString::fromLatin1(Constants::Icons::XMAKE_FILE_OVERLAY) });
        setListInProject(true);
    }
} // namespace XMakeProjectManager::Internal

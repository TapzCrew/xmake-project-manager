// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

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
    XMakeDependencyNode::XMakeDependencyNode(const Utils::FilePath &directory) {
        setPriority(ProjectExplorer::Node::DefaultFolderPriority + 5);
        setIcon(QIcon { ProjectExplorer::Constants::FILEOVERLAY_MODULES });
        setFilePath(directory);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    QString XMakeDependencyNode::displayName() const { return m_name; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    void XMakeDependencyNode::setDisplayName(const QString &name) { m_name = name; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeDependencyNode::icon() const -> QIcon { return m_icon; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeDependencyNode::setIcon(const QIcon icon) -> void { m_icon = icon; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeTargetNode::XMakeTargetNode(const Utils::FilePath &directory,
                                     const QString &name,
                                     ProjectExplorer::ProductType type)
        : ProjectExplorer::ProjectNode { directory }, m_name { name } {
        setPriority(Node::DefaultProjectPriority + 900);
        setIcon(":/projectexplorer/images/build.png");
        setListInProject(false);
        setShowWhenEmpty(true);
        setProductType(type);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeTargetNode::build() -> void {
        auto project = getProject();
        auto target  = project ? project->activeTarget() : nullptr;

        if (target)
            static_cast<XMakeBuildSystem *>(target->buildSystem())
                ->xmakeBuildConfiguration()
                ->build(m_name);
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

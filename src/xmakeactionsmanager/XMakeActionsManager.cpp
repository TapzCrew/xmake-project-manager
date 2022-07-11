// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeActionsManager.hpp"

#include <project/XMakeBuildSystem.hpp>
#include <project/projecttree/XMakeProjectNodes.hpp>

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/coreconstants.h>

#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projecttree.h>

#include <utils/parameteraction.h>

#include <QStringLiteral>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeActionsManager::XMakeActionsManager()
        : m_configure_action_context_menu { tr("Configure") }, m_configure_action_menu { tr(
                                                                   "Configure") },
          m_build_target_context_action { tr("Build"),
                                          tr("Build \"%1\""),
                                          Utils::ParameterAction::AlwaysEnabled } {
        const auto global_context  = Core::Context { Core::Constants::C_GLOBAL };
        const auto project_context = Core::Context { Constants::Project::ID };

        auto *m_project =
            Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_PROJECTCONTEXT);
        auto *m_subproject =
            Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_SUBPROJECTCONTEXT);

        using CommandPtr = Core::Command *;

        {
            auto *command = Core::ActionManager::registerAction(&m_configure_action_menu,
                                                                "XMakeProject.Configure",
                                                                project_context);

            m_project->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);
            m_subproject->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);

            connect(&m_configure_action_menu,
                    &QAction::triggered,
                    this,
                    &XMakeActionsManager::configureCurrentProject);
        }

        {
            auto *command = Core::ActionManager::registerAction(&m_build_target_context_action,
                                                                "XMake.BuildTargetContextMenu",
                                                                project_context);

            command->setAttribute(Core::Command::CA_Hide);
            command->setAttribute(Core::Command::CA_UpdateText);
            command->setDescription(m_build_target_context_action.text());

            Core::ActionManager::actionContainer(ProjectExplorer::Constants::M_SUBPROJECTCONTEXT)
                ->addAction(command, ProjectExplorer::Constants::G_PROJECT_BUILD);

            connect(ProjectExplorer::ProjectTree::instance(),
                    &ProjectExplorer::ProjectTree::currentNodeChanged,
                    this,
                    &XMakeActionsManager::updateContextActions);

            connect(&m_build_target_context_action, &Utils::ParameterAction::triggered, this, [] {
                auto *bs = qobject_cast<XMakeBuildSystem *>(
                    ProjectExplorer::ProjectTree::currentBuildSystem());

                if (bs != nullptr) {
                    auto *target_node = dynamic_cast<XMakeTargetNode *>(
                        ProjectExplorer::ProjectTree::currentNode());
                    target_node->build();
                }
            });
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeActionsManager::configureCurrentProject() -> void {
        auto *bs =
            dynamic_cast<XMakeBuildSystem *>(ProjectExplorer::ProjectTree::currentBuildSystem());
        QTC_ASSERT(bs, return );

        if (ProjectExplorer::ProjectExplorerPlugin::saveModifiedFiles()) bs->configure();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeActionsManager::updateContextActions() -> void {
        const auto *target_node =
            dynamic_cast<const XMakeTargetNode *>(ProjectExplorer::ProjectTree::currentNode());
        const auto target_display_name = target_node ? target_node->displayName() : QString {};

        m_build_target_context_action.setParameter(target_display_name);
        m_build_target_context_action.setEnabled(target_node);
        m_build_target_context_action.setVisible(target_node);
    }
} // namespace XMakeProjectManager::Internal

// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeProject.hpp"

#include <XMakeProjectConstant.hpp>

#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <coreplugin/icontext.h>

#include <projectexplorer/deploymentdata.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/projectimporter.h>
#include <projectexplorer/task.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProject::XMakeProject(Utils::FilePath path)
        : ProjectExplorer::Project { QLatin1String { Constants::Project::MIMETYPE },
                                     std::move(path) } {
        setId(Constants::Project::ID);
        setProjectLanguages(Core::Context(ProjectExplorer::Constants::CXX_LANGUAGE_ID,
                                          ProjectExplorer::Constants::C_LANGUAGE_ID));
        setDisplayName(projectDirectory().fileName());
        setCanBuildProducts();
        setHasMakeInstallEquivalent(true);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProject::projectIssues(const ProjectExplorer::Kit *k) const
        -> ProjectExplorer::Tasks {
        auto result = Project::projectIssues(k);

        if (!XMakeToolKitAspect::isValid(k))
            result.append(createProjectTask(ProjectExplorer::Task::TaskType::Error,
                                            tr("No XMake tool set.")));
        if (ProjectExplorer::ToolChainKitAspect::toolChains(k).isEmpty())
            result.append(createProjectTask(ProjectExplorer::Task::TaskType::Warning,
                                            tr("No compilers set in kit.")));

        return result;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProject::projectImporter() const -> ProjectExplorer::ProjectImporter * {
        if (!m_project_importer)
            m_project_importer = std::make_unique<XMakeProjectImporter>(projectFilePath());

        return m_project_importer.get();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProject::deploymentKnowledge() const -> ProjectExplorer::DeploymentKnowledge {
        return ProjectExplorer::DeploymentKnowledge::Bad;
    }
} // namespace XMakeProjectManager::Internal

#include "XMakeProject.moc"

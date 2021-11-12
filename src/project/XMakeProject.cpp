#include "XMakeProject.hpp"

#include <XMakeProjectConstant.hpp>

#include <project/XMakeProjectImporter.hpp>

#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <projectexplorer/deploymentdata.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectimporter.h>
#include <projectexplorer/task.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProject::XMakeProject(Utils::FilePath path)
        : ProjectExplorer::Project { QLatin1String { Constants::Project::MIMETYPE },
                                     std::move(path) } {}

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProject::~XMakeProject() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProject::projectIssues(const ProjectExplorer::Kit *k) const
        -> ProjectExplorer::Tasks {
        auto result = Project::projectIssues(k);

        if (!XMakeToolKitAspect::isValid(k))
            result.append(createProjectTask(ProjectExplorer::Task::TaskType::Error,
                                            tr("No XMake tool set.")));

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

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProject::makeInstallCommand([[maybe_unused]] const ProjectExplorer::Target *target,
                                          [[maybe_unused]] const QString &install_root)
        -> ProjectExplorer::MakeInstallCommand {
        return {};
    }
} // namespace XMakeProjectManager::Internal

#include "XMakeProject.moc"

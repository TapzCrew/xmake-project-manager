#pragma once

#include <projectexplorer/deploymentdata.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectimporter.h>
#include <projectexplorer/task.h>

#include <utils/filepath.h>

#include <type_traits>

namespace XMakeProjectManager::Internal {
    class XMakeProject final: public ProjectExplorer::Project {
        Q_OBJECT

      public:
        explicit XMakeProject(Utils::FilePath path);
        ~XMakeProject() override;

        XMakeProject(XMakeProject &&)      = delete;
        XMakeProject(const XMakeProject &) = delete;

        XMakeProject &operator=(XMakeProject &&) = delete;
        XMakeProject &operator=(const XMakeProject &) = delete;

        ProjectExplorer::Tasks projectIssues(const ProjectExplorer::Kit *k) const override;
        ProjectExplorer::ProjectImporter *projectImporter() const override;

      private:
        ProjectExplorer::DeploymentKnowledge deploymentKnowledge() const override;
        ProjectExplorer::MakeInstallCommand
            makeInstallCommand(const ProjectExplorer::Target *target,
                               const QString &install_root) override;
    };
} // namespace XMakeProjectManager::Internal

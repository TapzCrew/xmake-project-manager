// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <projectexplorer/deploymentdata.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/project.h>
#include <projectexplorer/projectimporter.h>
#include <projectexplorer/task.h>

#include <utils/filepath.h>

#include <type_traits>

#include <project/XMakeProjectImporter.hpp>

namespace XMakeProjectManager::Internal {
    class XMakeProject final: public ProjectExplorer::Project {
        Q_OBJECT

      public:
        explicit XMakeProject(Utils::FilePath path);

        ProjectExplorer::Tasks projectIssues(const ProjectExplorer::Kit *k) const override;
        ProjectExplorer::ProjectImporter *projectImporter() const override;

      private:
        ProjectExplorer::DeploymentKnowledge deploymentKnowledge() const override;

        mutable std::unique_ptr<XMakeProjectImporter> m_project_importer;
    };
} // namespace XMakeProjectManager::Internal

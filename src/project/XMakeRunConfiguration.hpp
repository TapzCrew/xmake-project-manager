// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <projectexplorer/desktoprunconfiguration.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/target.h>

namespace XMakeProjectManager::Internal {
    class XMakeRunConfiguration final: public ProjectExplorer::RunConfiguration {
      public:
        XMakeRunConfiguration(ProjectExplorer::Target *target, Utils::Id id);

      private:
        void updateTargetInformation();
    };

    class XMakeRunConfigurationFactory final: public ProjectExplorer::RunConfigurationFactory {
      public:
        XMakeRunConfigurationFactory();
    };
} // namespace XMakeProjectManager::Internal

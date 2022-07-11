// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeRunConfiguration.hpp"

#include <XMakeProjectConstant.hpp>

#include <projectexplorer/buildsystem.h>
#include <projectexplorer/desktoprunconfiguration.h>
#include <projectexplorer/environmentaspect.h>
#include <projectexplorer/localenvironmentaspect.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/runconfigurationaspects.h>

#include <utils/environment.h>
#include <utils/hostosinfo.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeRunConfiguration::XMakeRunConfiguration(ProjectExplorer::Target *target, Utils::Id id)
        : ProjectExplorer::RunConfiguration { target, id } {
        auto env_aspect = addAspect<ProjectExplorer::LocalEnvironmentAspect>(target);

        addAspect<ProjectExplorer::ExecutableAspect>(target,
                                                     ProjectExplorer::ExecutableAspect::RunDevice);
        addAspect<ProjectExplorer::ArgumentsAspect>(macroExpander());
        addAspect<ProjectExplorer::WorkingDirectoryAspect>(macroExpander(), env_aspect);
        addAspect<ProjectExplorer::TerminalAspect>();

        auto lib_aspect = addAspect<ProjectExplorer::UseLibraryPathsAspect>();
        connect(lib_aspect,
                &ProjectExplorer::UseLibraryPathsAspect::changed,
                env_aspect,
                &ProjectExplorer::EnvironmentAspect::environmentChanged);

        if (Utils::HostOsInfo::isMacHost()) {
            auto dyld_aspect = addAspect<ProjectExplorer::UseDyldSuffixAspect>();
            connect(dyld_aspect,
                    &ProjectExplorer::UseLibraryPathsAspect::changed,
                    env_aspect,
                    &ProjectExplorer::EnvironmentAspect::environmentChanged);

            env_aspect->addModifier([dyld_aspect](Utils::Environment &env) {
                if (dyld_aspect->value())
                    env.set(QLatin1String { "DYLD_IMAGE_SUFFIX" }, QLatin1String { "_debug" });
            });
        }

        env_aspect->addModifier([this, lib_aspect](Utils::Environment &env) {
            ProjectExplorer::BuildTargetInfo b_ti = buildTargetInfo();
            if (b_ti.runEnvModifier) b_ti.runEnvModifier(env, lib_aspect->value());
        });

        setUpdater([this] { updateTargetInformation(); });

        connect(target,
                &ProjectExplorer::Target::buildSystemUpdated,
                this,
                &ProjectExplorer::RunConfiguration::update);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeRunConfiguration::updateTargetInformation() -> void {
        if (!activeBuildSystem()) return;

        auto b_ti            = buildTargetInfo();
        auto terminal_aspect = aspect<ProjectExplorer::TerminalAspect>();
        terminal_aspect->setUseTerminalHint(b_ti.usesTerminal);

        aspect<ProjectExplorer::ExecutableAspect>()->setExecutable(b_ti.targetFilePath);
        aspect<ProjectExplorer::WorkingDirectoryAspect>()->setDefaultWorkingDirectory(
            b_ti.workingDirectory);
        Q_EMIT aspect<ProjectExplorer::LocalEnvironmentAspect>()->environmentChanged();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeRunConfigurationFactory::XMakeRunConfigurationFactory() {
        registerRunConfiguration<XMakeRunConfiguration>(
            "XMakeProjectManager.XMakeRunConfiguration");

        addSupportedProjectType(Constants::Project::ID);
        addSupportedTargetDeviceType(ProjectExplorer::Constants::DESKTOP_DEVICE_TYPE);
    }
} // namespace XMakeProjectManager::Internal

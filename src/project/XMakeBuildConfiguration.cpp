// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeBuildConfiguration.hpp"

#include <XMakeProjectConstant.hpp>

#include <project/XMakeBuildStep.hpp>
#include <project/XMakeBuildSystem.hpp>

#include <qtsupport/qtcppkitinfo.h>
#include <qtsupport/qtkitinformation.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/buildinfo.h>
#include <projectexplorer/buildmanager.h>
#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/project.h>
#include <projectexplorer/target.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildConfiguration::XMakeBuildConfiguration(ProjectExplorer::Target *target, Utils::Id id)
        : ProjectExplorer::BuildConfiguration { target, id } {
        appendInitialBuildStep(Constants::XMAKE_BUILD_STEP_ID);
        appendInitialCleanStep(Constants::XMAKE_BUILD_STEP_ID);

        setInitializer([this, target](const auto &info) {
            m_build_type = xmakeBuildType(info.typeName);

            m_parameters = QString { "-v -m %1" }.arg(info.typeName);

            auto *kit     = target->kit();
            auto kit_info = QtSupport::CppKitInfo { kit };
            if (kit_info.qtVersion && !kit_info.qtVersion->prefix().isEmpty()) {
                m_parameters +=
                    QString { " --qt=\"%1\"" }.arg(kit_info.qtVersion->prefix().toString());
                qDebug() << m_parameters;
            }

            if (info.buildDirectory.isEmpty())
                setBuildDirectory(shadowBuildDirectory(target->project()->projectFilePath(),
                                                       kit,
                                                       info.displayName,
                                                       info.buildType));

            m_build_system = std::make_unique<XMakeBuildSystem>(this);
        });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildConfiguration::shadowBuildDirectory(
        const Utils::FilePath &project_file_path,
        const ProjectExplorer::Kit *kit,
        const QString &bc_name,
        ProjectExplorer::BuildConfiguration::BuildType build_type) -> Utils::FilePath {
        if (project_file_path.isEmpty()) return {};

        const auto project_name = project_file_path.parentDir().fileName();

        return ProjectExplorer::BuildConfiguration::buildDirectoryFromTemplate(
            ProjectExplorer::Project::projectDirectory(project_file_path),
            project_file_path,
            project_name,
            kit,
            bc_name,
            build_type,
            ProjectExplorer::BuildConfiguration::ReplaceSpaces);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildConfiguration::buildSystem() const -> ProjectExplorer::BuildSystem * {
        return m_build_system.get();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildConfiguration::build(const QString &target) -> void {
        const auto &build_steps = *buildSteps();

        auto *xmake_build_step = qobject_cast<XMakeBuildStep *>(
            Utils::findOrDefault(buildSteps()->steps(), [](const auto *bs) {
                return bs->id() == Constants::XMAKE_BUILD_STEP_ID;
            }));

        auto original_build_target = QString {};
        if (xmake_build_step) {
            original_build_target = xmake_build_step->targetName();
            xmake_build_step->setBuildTarget(target);
        }

        ProjectExplorer::BuildManager::buildList(buildSteps());

        if (xmake_build_step) xmake_build_step->setBuildTarget(target);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildConfiguration::xmakeConfigArgs() -> QStringList {
        return Utils::ProcessArgs::splitArgs(m_parameters);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildConfiguration::toMap() const -> QVariantMap {
        auto data = ProjectExplorer::BuildConfiguration::toMap();
        data[QString::fromLatin1(Constants::BuildConfiguration::BUILD_TYPE_KEY)] =
            xmakeBuildTypeName(m_build_type);
        data[QString::fromLatin1(Constants::BuildConfiguration::PARAMETERS_KEY)] = m_parameters;

        return data;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildConfiguration::fromMap(const QVariantMap &map) -> bool {
        const auto result = ProjectExplorer::BuildConfiguration::fromMap(map);

        m_build_system = std::make_unique<XMakeBuildSystem>(this);
        m_build_type   = xmakeBuildType(
            map.value(QString::fromLatin1(Constants::BuildConfiguration::BUILD_TYPE_KEY))
                .toString());
        m_parameters = map.value(QString::fromLatin1(Constants::BuildConfiguration::PARAMETERS_KEY))
                           .toString();

        return result;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildConfiguration::createConfigWidget() -> ProjectExplorer::NamedWidget * {
        return nullptr;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto createBuildInfo(XMakeBuildType type) -> ProjectExplorer::BuildInfo {
        auto b_info        = ProjectExplorer::BuildInfo {};
        b_info.typeName    = xmakeBuildTypeName(type);
        b_info.displayName = xmakeBuildTypeDisplayName(type);
        b_info.buildType   = buildType(type);

        return b_info;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildConfigurationFactory::XMakeBuildConfigurationFactory() {
        registerBuildConfiguration<XMakeBuildConfiguration>(Constants::XMAKE_BUILD_CONFIG_ID);
        setSupportedProjectType(Constants::Project::ID);
        setSupportedProjectMimeTypeName(QString::fromLatin1(Constants::Project::MIMETYPE));

        setBuildGenerator([](const ProjectExplorer::Kit *kit,
                             const Utils::FilePath &project_path,
                             bool for_setup) {
            auto result = QList<ProjectExplorer::BuildInfo> {};

            auto path =
                for_setup ? ProjectExplorer::Project::projectDirectory(project_path) : project_path;

            for (const auto &b_type : { XMakeBuildType::Debug,
                                        XMakeBuildType::Release,
                                        XMakeBuildType::Release_Dbg,
                                        XMakeBuildType::Min_Size_Rel,
                                        XMakeBuildType::Check,
                                        XMakeBuildType::Profile,
                                        XMakeBuildType::Coverage,
#ifndef Q_OS_WINDOWS
                                        XMakeBuildType::Valgrind,
#endif
                                        XMakeBuildType::Asan,
                                        XMakeBuildType::Tsan,
                                        XMakeBuildType::Lsan,
                                        XMakeBuildType::Ubsan }) {
                auto b_info = createBuildInfo(b_type);

                if (for_setup)
                    b_info.buildDirectory =
                        XMakeBuildConfiguration::shadowBuildDirectory(project_path,
                                                                      kit,
                                                                      b_info.typeName,
                                                                      b_info.buildType);

                result << b_info;
            }

            return result;
        });
    }
} // namespace XMakeProjectManager::Internal

// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeBuildSystem.hpp"
#include "utils/filepath.h"

#include <project/XMakeBuildConfiguration.hpp>

#include <settings/general/Settings.hpp>

#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <utils/qtcassert.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>
#include <projectexplorer/taskhub.h>

#include <qtsupport/qtcppkitinfo.h>
#include <qtsupport/qtkitinformation.h>

#include <qmljs/qmljsdialect.h>
#include <qmljs/qmljsmodelmanagerinterface.h>

#define LEAVE_IF_BUSY()                                  \
    {                                                    \
        if (m_parse_guard.guardsProject()) return false; \
    }

#define LOCK() \
    { m_parse_guard = guardParsingRun(); }

#define UNLOCK(success)                             \
    {                                               \
        if (success) m_parse_guard.markAsSuccess(); \
        m_parse_guard = {};                         \
    }

namespace XMakeProjectManager::Internal {
    static Q_LOGGING_CATEGORY(xmake_build_system_log, "qtc.xmake.buildsystem", QtDebugMsg);

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildSystem::XMakeBuildSystem(XMakeBuildConfiguration *build_conf)
        : ProjectExplorer::BuildSystem { build_conf }, m_parser { XMakeToolKitAspect::xmakeToolId(
                                                                      build_conf->kit()),
                                                                  build_conf->environment(),
                                                                  project() },
          m_kit_info { nullptr } {
        init();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::triggerParsing() -> void {
        qCDebug(xmake_build_system_log) << "Trigger parsing";

        parseProject();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::configure() -> bool {
        LEAVE_IF_BUSY();

        LOCK();
        qCDebug(xmake_build_system_log) << "Configure";

        if (m_parser.configure(projectDirectory(),
                               buildConfiguration()->buildDirectory(),
                               configArgs(false))) {
            return true;
        }
        UNLOCK(false);

        Q_EMIT errorOccurred();

        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::wipe() -> bool {
        LEAVE_IF_BUSY();

        LOCK();
        qCDebug(xmake_build_system_log) << "Wipe";

        if (m_parser.wipe(projectDirectory(),
                          buildConfiguration()->buildDirectory(),
                          configArgs(true))) {
            return true;
        }

        UNLOCK(false);

        Q_EMIT errorOccurred();

        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::xmakeBuildConfiguration() -> XMakeBuildConfiguration * {
        return dynamic_cast<XMakeBuildConfiguration *>(buildConfiguration());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::makeInstallCommand([[maybe_unused]] const Utils::FilePath &install_root)
        const -> ProjectExplorer::MakeInstallCommand {
        return {};
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::init() -> void {
        qCDebug(xmake_build_system_log) << "Init";

        connect(buildConfiguration()->target(), &ProjectExplorer::Target::kitChanged, this, [this] {
            updateKit(kit());
        });

        connect(xmakeBuildConfiguration(),
                &XMakeBuildConfiguration::buildDirectoryChanged,
                this,
                [this] {
                    updateKit(kit());
                    triggerParsing();
                });

        connect(xmakeBuildConfiguration(),
                &XMakeBuildConfiguration::parametersChanged,
                this,
                [this] {
                    updateKit(kit());
                    wipe();
                });

        connect(xmakeBuildConfiguration(),
                &XMakeBuildConfiguration::environmentChanged,
                this,
                [this] { m_parser.setEnvironment(buildConfiguration()->environment()); });

        connect(project(), &ProjectExplorer::Project::projectFileIsDirty, this, [this]() {
            if (buildConfiguration()->isActive()) parseProject();
        });

        connect(&m_parser,
                &XMakeProjectParser::parsingCompleted,
                this,
                &XMakeBuildSystem::parsingCompleted);

        connect(&m_intro_watcher, &Utils::FileSystemWatcher::fileChanged, this, [this] {
            if (buildConfiguration()->isActive()) parseProject();
        });

        updateKit(kit());

        m_intro_watcher.addDirectory(buildConfiguration()
                                         ->buildDirectory()
                                         .pathAppended(
                                             QString::fromLatin1(Constants::XMAKE_INFO_DIR))
                                         .toString(),
                                     Utils::FileSystemWatcher::WatchModifiedDate);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::parseProject() -> bool {
        QTC_ASSERT(buildConfiguration(), return false);

        const auto xmake = XMakeToolKitAspect::xmakeTool(xmakeBuildConfiguration()->kit());

        if (xmake && xmake->autorun()) return configure();

        LEAVE_IF_BUSY();
        LOCK();

        qCDebug(xmake_build_system_log) << "Starting parser";

        const auto result =
            m_parser.parse(projectDirectory(), buildConfiguration()->buildDirectory());
        if (result) return true;

        UNLOCK(false);

        Q_EMIT errorOccurred();

        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::updateKit(ProjectExplorer::Kit *kit) -> void {
        QTC_ASSERT(kit, return);

        m_kit_info = QtSupport::CppKitInfo { kit };
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::parsingCompleted(bool success) -> void {
        if (!success) {
            ProjectExplorer::TaskHub::addTask(
                ProjectExplorer::BuildSystemTask { ProjectExplorer::Task::Error,
                                                   tr("XMake introspection: Parsing failed") });
            UNLOCK(false);

            emitBuildSystemUpdated();

            return;
        }

        setRootProjectNode(m_parser.takeProjectNode());

        if (kit() && buildConfiguration()) {
            auto kit_info = QtSupport::CppKitInfo { kit() };

            m_cpp_code_model_updater.update({ project(),
                                              QtSupport::CppKitInfo { kit() },
                                              buildConfiguration()->environment(),
                                              m_parser.buildProjectParts(kit_info) });

            updateQMLCodeModel();
        }

        setApplicationTargets(m_parser.appTargets());

        UNLOCK(true);

        emitBuildSystemUpdated();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::XMakeBuildSystem::kit() -> ProjectExplorer::Kit * {
        return buildConfiguration()->kit();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::configArgs(bool is_setup) -> QStringList {
        return QStringList {} + m_pending_config_args +
               xmakeBuildConfiguration()->xmakeConfigArgs();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::updateQMLCodeModel() -> void {
        auto model_manager = QmlJS::ModelManagerInterface::instance();
        if (!model_manager) return;

        auto project = this->project();

        auto rcc          = Utils::FilePaths {};
        auto project_info = model_manager->defaultProjectInfoForProject(project, rcc);
        project_info.importPaths.clear();

        for (const auto &import : m_parser.qmlImportPaths()) {
            qCDebug(xmake_build_system_log) << "QML IMPORT PATH: " << import;
            project_info.importPaths.maybeInsert(Utils::FilePath::fromString(import),
                                                 QmlJS::Dialect::Qml);
        }

        project->setProjectLanguage(ProjectExplorer::Constants::QMLJS_LANGUAGE_ID,
                                    !project_info.sourceFiles.empty());

        model_manager->updateProjectInfo(project_info, project);
    }
} // namespace XMakeProjectManager::Internal

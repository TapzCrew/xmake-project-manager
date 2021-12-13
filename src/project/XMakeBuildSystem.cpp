#include "XMakeBuildSystem.hpp"

#include <project/XMakeBuildConfiguration.hpp>

#include <settings/general/Settings.hpp>

#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <kithelper/KitHelper.hpp>

#include <utils/qtcassert.h>

#include <projectexplorer/buildconfiguration.h>
#include <projectexplorer/target.h>
#include <projectexplorer/taskhub.h>

#include <qtsupport/qtcppkitinfo.h>
#include <qtsupport/qtkitinformation.h>

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
    static Q_LOGGING_CATEGORY(xmake_build_system_log, "qt.xmake.buildsystem", QtDebugMsg);

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildSystem::XMakeBuildSystem(XMakeBuildConfiguration *build_conf)
        : ProjectExplorer::BuildSystem { build_conf }, m_parser { XMakeToolKitAspect::xmakeToolId(
                                                                      build_conf->kit()),
                                                                  build_conf->environment(),
                                                                  project() } {
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

        qCDebug(xmake_build_system_log) << "Configure";

        LOCK();
        if (m_parser.configure(projectDirectory(),
                               buildConfiguration()->buildDirectory(),
                               configArgs(false))) {
            return true;
        }
        UNLOCK(false);

        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::wipe() -> bool {
        LEAVE_IF_BUSY();

        qCDebug(xmake_build_system_log) << "Wipe";

        LOCK();
        // TODO

        UNLOCK(false);

        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::xmakeBuildConfiguration() -> XMakeBuildConfiguration * {
        return dynamic_cast<XMakeBuildConfiguration *>(buildConfiguration());
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
            if(buildConfiguration()->isActive())
                parseProject();
        });

        connect(&m_parser, &XMakeProjectParser::parsingCompleted, this, &XMakeBuildSystem::parsingCompleted);

        connect(&m_intro_watcher, &Utils::FileSystemWatcher::fileChanged, this, [this] {
            if (buildConfiguration()->isActive()) parseProject();
        });

        updateKit(kit());

        m_intro_watcher.addFile(buildConfiguration()
                                    ->buildDirectory()
                                    .pathAppended(QString::fromLatin1(Constants::XMAKE_INFO_DIR))
                                    .toString(),
                                Utils::FileSystemWatcher::WatchModifiedDate);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::parseProject() -> bool {
        QTC_ASSERT(buildConfiguration(), return false);

        if (!isSetup(buildConfiguration()->buildDirectory()) &&
            Settings::instance()->autorunXMake().value())
            return configure();

        LEAVE_IF_BUSY();
        LOCK();

        qCDebug(xmake_build_system_log) << "Starting parser";

        if (m_parser.parse(projectDirectory(), buildConfiguration()->buildDirectory())) return true;

        UNLOCK(false);

        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::updateKit(ProjectExplorer::Kit *kit) -> void {
        QTC_ASSERT(kit, return );

        m_kit_data = KitHelper::kitData(kit);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildSystem::parsingCompleted(bool success) -> void {

        if (!success) {
            ProjectExplorer::TaskHub::addTask(
                ProjectExplorer::BuildSystemTask { ProjectExplorer::Task::Error,
                                                   tr("XMake build: Parsing failed") });
            UNLOCK(false);

            emitBuildSystemUpdated();

            return;
        }

        setRootProjectNode(m_parser.takeProjectNode());

        if (kit() && buildConfiguration()) {
            auto kit_info = ProjectExplorer::KitInfo { kit() };
            /*m_cpp_code_model_updater.update(
                { project(),
                  QtSupport::CppKitInfo { kit() },
                  buildConfiguration()->environment(),
                  m_parser.buildProjectParts(kit_info.cxxToolChain, kit_info.cToolChain) });*/
        }

        // setApplicationTargets(m_parser.appsTargets());

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
        const auto &params = xmakeBuildConfiguration()->parameters();

        if (!is_setup) return m_pending_config_args + xmakeBuildConfiguration()->xmakeConfigArgs();

        return QStringList {} + m_pending_config_args +
               xmakeBuildConfiguration()->xmakeConfigArgs();
    }
} // namespace XMakeProjectManager::Internal

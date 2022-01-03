#include "XMakeProjectPlugin.hpp"

#include <XMakeProjectConstant.hpp>

#include <project/XMakeBuildConfiguration.hpp>
#include <project/XMakeBuildStep.hpp>
#include <project/XMakeProject.hpp>
#include <project/XMakeRunConfiguration.hpp>

#include <xmakeactionsmanager/XMakeActionsManager.hpp>

#include <settings/general/Settings.hpp>
#include <settings/tools/ToolsSettingsAccessor.hpp>
#include <settings/tools/ToolsSettingsPage.hpp>
#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <coreplugin/fileiconprovider.h>
#include <coreplugin/icore.h>

#include <projectexplorer/projectmanager.h>
#include <projectexplorer/runconfiguration.h>
#include <projectexplorer/runcontrol.h>

namespace XMakeProjectManager::Internal {
    class XMakeProjectPlugin::XMakeProjectPluginPrivate final: public QObject {
        Q_OBJECT

      public:
        XMakeProjectPluginPrivate()
            : m_xmake_run_worker_factory {
                  ProjectExplorer::RunWorkerFactory::make<ProjectExplorer::SimpleTargetRunner>(),
                  { ProjectExplorer::Constants::NORMAL_RUN_MODE },
                  { m_run_configuration_factory.runConfigurationId() }
              } {
            XMakeTools::setTools(m_tools_settings.loadXMakeTools(Core::ICore::dialogParent()));
            connect(Core::ICore::instance(),
                    &Core::ICore::saveSettingsRequested,
                    this,
                    &XMakeProjectPluginPrivate::saveAll);
        }

        ~XMakeProjectPluginPrivate() override = default;

        XMakeProjectPluginPrivate(XMakeProjectPluginPrivate &&)      = delete;
        XMakeProjectPluginPrivate(const XMakeProjectPluginPrivate &) = delete;

        XMakeProjectPluginPrivate &operator=(XMakeProjectPluginPrivate &&) = delete;
        XMakeProjectPluginPrivate &operator=(const XMakeProjectPluginPrivate &) = delete;

      private:
        void saveAll() {
            m_tools_settings.saveXMakeTools(XMakeTools::tools(), Core::ICore::dialogParent());
        }

        GeneralSettingsPage m_general_settings_page;
        ToolsSettingsPage m_tools_settings_page;
        ToolsSettingsAccessor m_tools_settings;
        XMakeToolKitAspect m_kit_aspect;

        XMakeBuildStepFactory m_build_step_factory;

        XMakeBuildConfigurationFactory m_build_configuration_factory;
        XMakeRunConfigurationFactory m_run_configuration_factory;

        XMakeActionsManager m_actions_manager;

        ProjectExplorer::RunWorkerFactory m_xmake_run_worker_factory;
    };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProjectPlugin::XMakeProjectPlugin() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                                        [[maybe_unused]] QString *error_messages)

        -> bool {
        m_pimpl.reset(new XMakeProjectPluginPrivate {});

        ProjectExplorer::ProjectManager::registerProjectType<XMakeProject>(
            QLatin1String { Constants::Project::MIMETYPE });

        Core::FileIconProvider::registerIconOverlayForFilename(
            QLatin1String { Constants::Icons::XMAKE_FILE_OVERLAY },
            "xmake.lua");

        return true;
    }

    auto XMakeProjectPlugin::XMakeProjectPluginPrivateDeleter::operator()(
        XMakeProjectPluginPrivate *p) -> void {
        delete p;
    }
} // namespace XMakeProjectManager::Internal

#include "XMakeProjectPlugin.moc"

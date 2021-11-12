#include "XMakeProjectPlugin.hpp"

#include <XMakeProjectConstant.hpp>

#include <project/XMakeProject.hpp>

#include <settings/general/Settings.hpp>
#include <settings/tools/ToolsSettingsAccessor.hpp>
#include <settings/tools/ToolsSettingsPage.hpp>
#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <coreplugin/fileiconprovider.h>
#include <coreplugin/icore.h>

#include <projectexplorer/projectmanager.h>

namespace XMakeProjectManager::Internal {
    class XMakeProjectPlugin::XMakeProjectPluginPrivate final: public QObject {
        Q_OBJECT

      public:
        XMakeProjectPluginPrivate() {
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
        XMakeToolKitAspect m_xmake_kit_aspect;
    };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProjectPlugin::XMakeProjectPlugin() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProjectPlugin::~XMakeProjectPlugin() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectPlugin::initialize([[maybe_unused]] const QStringList &arguments,
                                        [[maybe_unused]] QString *error_messages)

        -> bool {
        m_pimpl = std::make_unique<XMakeProjectPluginPrivate>();

        ProjectExplorer::ProjectManager::registerProjectType<XMakeProject>(
            QLatin1String { Constants::Project::MIMETYPE });

        Core::FileIconProvider::registerIconOverlayForFilename(
            QLatin1String { Constants::Icons::XMAKE },
            "XMake.lua");

        return true;
    }
} // namespace XMakeProjectManager::Internal

#include "XMakeProjectPlugin.moc"

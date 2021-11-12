#pragma once

#include <memory>

#include <extensionsystem/iplugin.h>

namespace XMakeProjectManager::Internal {
    class XMakeProjectPlugin final: public ExtensionSystem::IPlugin {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "XMakeProjectManager.json")

      public:
        XMakeProjectPlugin();
        ~XMakeProjectPlugin() override;

        XMakeProjectPlugin(XMakeProjectPlugin &&)      = delete;
        XMakeProjectPlugin(const XMakeProjectPlugin &) = delete;

        XMakeProjectPlugin &operator=(XMakeProjectPlugin &&) = delete;
        XMakeProjectPlugin &operator=(const XMakeProjectPlugin &) = delete;

      private:
        class XMakeProjectPluginPrivate;

        bool initialize(const QStringList &arguments, QString *error_message) override;

        std::unique_ptr<XMakeProjectPluginPrivate> m_pimpl;
    };
} // namespace XMakeProjectManager::Internal

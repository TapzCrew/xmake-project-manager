#pragma once

#include <memory>

#include <extensionsystem/iplugin.h>

namespace XMakeProjectManager::Internal {
    class XMakeProjectPlugin final: public ExtensionSystem::IPlugin {
        Q_OBJECT
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "XMakeProjectManager.json")

      public:
        XMakeProjectPlugin();

      private:
        class XMakeProjectPluginPrivate;
        struct XMakeProjectPluginPrivateDeleter {
            void operator()(XMakeProjectPluginPrivate *p);
        };

        bool initialize(const QStringList &arguments, QString *error_message) override;

        std::unique_ptr<XMakeProjectPluginPrivate, XMakeProjectPluginPrivateDeleter> m_pimpl;
    };
} // namespace XMakeProjectManager::Internal

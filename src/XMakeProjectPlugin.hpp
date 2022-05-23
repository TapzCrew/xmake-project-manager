// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

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

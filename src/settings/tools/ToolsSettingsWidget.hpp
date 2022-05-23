// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

#include <settings/tools/ToolsModel.hpp>

namespace XMakeProjectManager::Internal {
    namespace Ui {
        class ToolsSettingsWidget;
    }

    class ToolsModel;
    class ToolItemSettings;
    class ToolTreeItem;
    class ToolsSettingsWidget final: public Core::IOptionsPageWidget {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::ToolsSettingsWidget)

      public:
        ToolsSettingsWidget();

      private:
        void apply() override;

        void cloneXMakeTool();
        void removeXMakeTool();
        void currentXMakeToolChanged(const QModelIndex &new_current);

        std::unique_ptr<Ui::ToolsSettingsWidget> m_ui;
        ToolsModel m_model;
        ToolItemSettings *m_item_settings = nullptr;
        ToolTreeItem *m_current_item      = nullptr;
    };
} // namespace XMakeProjectManager::Internal

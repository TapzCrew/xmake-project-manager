#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

namespace XMakeProjectManager::Internal {
    class ToolsSettingsPage final: public Core::IOptionsPage {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::ToolsSettingsPage)

      public:
        ToolsSettingsPage();
        ~ToolsSettingsPage() override;

        ToolsSettingsPage(ToolsSettingsPage &&)      = delete;
        ToolsSettingsPage(const ToolsSettingsPage &) = delete;

        ToolsSettingsPage &operator=(ToolsSettingsPage &&) = delete;
        ToolsSettingsPage &operator=(const ToolsSettingsPage &) = delete;
    };
} // namespace XMakeProjectManager::Internal

#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

namespace XMakeProjectManager::Internal {
    class ToolsSettingsPage final: public Core::IOptionsPage {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::ToolsSettingsPage)

      public:
        ToolsSettingsPage();
    };
} // namespace XMakeProjectManager::Internal

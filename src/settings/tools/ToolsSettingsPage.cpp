#include "ToolsSettingsPage.hpp"

#include <settings/tools/ToolsSettingsWidget.hpp>

#include <XMakeProjectConstant.hpp>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolsSettingsPage::ToolsSettingsPage() {
        setId(Constants::SettingsPage::TOOLS_ID);
        setDisplayName(tr("Tools"));
        setCategory(Constants::SettingsPage::CATEGORY);
        setWidgetCreator([]() { return new ToolsSettingsWidget {}; });
    }
} // namespace XMakeProjectManager::Internal

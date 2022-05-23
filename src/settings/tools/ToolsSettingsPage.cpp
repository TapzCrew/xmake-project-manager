// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

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

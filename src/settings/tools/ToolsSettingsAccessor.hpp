// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <exewrappers/XMakeTools.hpp>

#include <utils/settingsaccessor.h>

namespace XMakeProjectManager::Internal {
    class ToolsSettingsAccessor final: public Utils::UpgradingSettingsAccessor {
      public:
        ToolsSettingsAccessor();

        void saveXMakeTools(const std::vector<XMakeTools::XMakeWrapperPtr> &tools, QWidget *parent);
        std::vector<XMakeTools::XMakeWrapperPtr> loadXMakeTools(QWidget *parent);
    };
} // namespace XMakeProjectManager::Internal

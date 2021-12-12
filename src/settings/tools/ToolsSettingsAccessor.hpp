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

#pragma once

#include <exewrappers/XMakeTools.hpp>

#include <utils/settingsaccessor.h>

namespace XMakeProjectManager::Internal {
    class ToolsSettingsAccessor final: public Utils::UpgradingSettingsAccessor {
      public:
        ToolsSettingsAccessor();
        ~ToolsSettingsAccessor() override;

        ToolsSettingsAccessor(ToolsSettingsAccessor &&)      = delete;
        ToolsSettingsAccessor(const ToolsSettingsAccessor &) = delete;

        ToolsSettingsAccessor &operator=(ToolsSettingsAccessor &&) = delete;
        ToolsSettingsAccessor &operator=(const ToolsSettingsAccessor &) = delete;

        void saveXMakeTools(std::span<const XMakeTools::XMakeWrapperPtr> tools, QWidget *parent);
        std::vector<XMakeTools::XMakeWrapperPtr> loadXMakeTools(QWidget *parent);
    };
} // namespace XMakeProjectManager::Internal

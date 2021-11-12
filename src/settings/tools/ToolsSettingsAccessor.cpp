#include "ToolsSettingsAccessor.hpp"

#include <XMakeProjectConstant.hpp>

#include <exewrappers/XMakeWrapper.hpp>

#include <app/app_version.h>

#include <coreplugin/icore.h>

#include <utils/fileutils.h>

#include <QCoreApplication>
#include <QVariantMap>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto entryName(int index) {
        return QString { QStringLiteral("%1%2") }
            .arg(QLatin1String { Constants::ToolsSettings::ENTRY_KEY })
            .arg(index);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolsSettingsAccessor::ToolsSettingsAccessor()
        : Utils::UpgradingSettingsAccessor(
              "QtCreatorXMakeTools",
              QCoreApplication::translate("XMakeProjectManager::XMakeToolManager", "XMake"),
              Core::Constants::IDE_DISPLAY_NAME) {
        setBaseFilePath(
            Core::ICore::userResourcePath(QLatin1String { Constants::ToolsSettings::FILENAME }));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolsSettingsAccessor::~ToolsSettingsAccessor() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto
        ToolsSettingsAccessor::saveXMakeTools(const std::vector<XMakeTools::XMakeWrapperPtr> &tools,
                                              QWidget *parent) -> void {
        auto data = QVariantMap {};

        auto entry_count = 0u;

        for (const auto &tool : tools) {
            data.insert(entryName(entry_count), toVariantMap(*tool));

            ++entry_count;
        }

        data.insert(QLatin1String { Constants::ToolsSettings::ENTRY_COUNT }, entry_count);
        saveSettings(data, parent);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsSettingsAccessor::loadXMakeTools(QWidget *parent)
        -> std::vector<XMakeTools::XMakeWrapperPtr> {
        const auto data = restoreSettings(parent);

        const auto entry_count =
            data.value(QLatin1String { Constants::ToolsSettings::ENTRY_COUNT }, 0).toInt();

        auto result = std::vector<XMakeTools::XMakeWrapperPtr> {};
        result.reserve(entry_count);

        for (auto tool_index = 0; tool_index < entry_count; ++tool_index) {
            auto name = entryName(tool_index);

            if (data.contains(name)) {
                const auto map = data[name].toMap();
                const auto type =
                    map.value(QLatin1String { Constants::ToolsSettings::TOOL_TYPE_KEY },
                              Constants::ToolsSettings::TOOL_TYPE_XMAKE); // TODO xrepo

                result.emplace_back(fromVariantMap(data[name].toMap()));
            }
        }

        return result;
    }
} // namespace XMakeProjectManager::Internal

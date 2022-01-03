#include "Settings.hpp"

#include <XMakeProjectConstant.hpp>

#include <utils/layoutbuilder.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    Settings::Settings() {
        setSettingsGroup("XMakeProjectManager");
        setAutoApply(false);

        m_autorun_xmake.setSettingsKey("xmake.autorun");
        m_autorun_xmake.setLabelText(tr("Autorun XMake"));
        m_autorun_xmake.setToolTip(tr("Automatically run XMake when needed."));

        m_accept_install_dependencies.setSettingsKey("xmake.accept_install_dependencies");
        m_accept_install_dependencies.setLabelText(tr("Accept dependencies install"));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto Settings::instance() -> Settings * {
        static auto settings = Settings {};

        return &settings;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    GeneralSettingsPage::GeneralSettingsPage() {
        setId(Constants::SettingsPage::GENERAL_ID);
        setDisplayName(tr("General"));
        setDisplayCategory("XMake");
        setCategory(Constants::SettingsPage::CATEGORY);
        setCategoryIconPath(
            Utils::FilePath::fromString(QLatin1String { Constants::Icons::XMAKE_BW }));
        setSettings(Settings::instance());

        setLayouter([](auto *widget) {
            auto &settings = *Settings::instance();

            Utils::Layouting::Column {
                settings.autorunXMake(),
                settings.acceptInstallDependencies(),
                Utils::Layouting::Stretch {}
            }.attachTo(widget);
        });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    GeneralSettingsPage::~GeneralSettingsPage() = default;
} // namespace XMakeProjectManager::Internal

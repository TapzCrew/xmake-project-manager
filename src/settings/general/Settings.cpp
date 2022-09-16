// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "Settings.hpp"

#include <XMakeProjectConstant.hpp>

#include <utils/layoutbuilder.h>

#include <unordered_map>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    Settings::Settings() {
        setSettingsGroup("XMakeProjectManager");
        setAutoApply(false);
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

            Utils::Layouting::Column { Utils::Layouting::st }.attachTo(widget);
        });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    GeneralSettingsPage::~GeneralSettingsPage() = default;
} // namespace XMakeProjectManager::Internal

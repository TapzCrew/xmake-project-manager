// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <utils/aspects.h>

#include <coreplugin/dialogs/ioptionspage.h>

namespace XMakeProjectManager::Internal {
    class Settings: public Utils::AspectContainer {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::Settings)
      public:
        Settings();

        static Settings *instance();

      private:
    };

    class GeneralSettingsPage final: public Core::IOptionsPage {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::GeneralSettingsPage)
      public:
        GeneralSettingsPage();
        ~GeneralSettingsPage();

        GeneralSettingsPage(GeneralSettingsPage &&)      = delete;
        GeneralSettingsPage(const GeneralSettingsPage &) = delete;

        GeneralSettingsPage &operator=(GeneralSettingsPage &&) = delete;
        GeneralSettingsPage &operator=(const GeneralSettingsPage &) = delete;
    };
} // namespace XMakeProjectManager::Internal

#include "Settings.inl"

#pragma once

#include <utils/aspects.h>

#include <coreplugin/dialogs/ioptionspage.h>

namespace XMakeProjectManager::Internal {
    class Settings: public Utils::AspectContainer {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::Settings)
      public:
        Settings();

        static Settings *instance();

        Utils::BoolAspect &autorunXMake() noexcept;
        const Utils::BoolAspect &autorunXMake() const noexcept;

        Utils::BoolAspect &acceptInstallDependencies() noexcept;
        const Utils::BoolAspect &acceptInstallDependencies() const noexcept;

      private:
        Utils::BoolAspect m_autorun_xmake;
        Utils::BoolAspect m_accept_install_dependencies;
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

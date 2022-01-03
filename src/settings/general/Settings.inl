#pragma once

#include "Settings.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Settings::autorunXMake() noexcept -> Utils::BoolAspect & { return m_autorun_xmake; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Settings::autorunXMake() const noexcept -> const Utils::BoolAspect & {
        return m_autorun_xmake;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Settings::acceptInstallDependencies() noexcept -> Utils::BoolAspect & {
        return m_accept_install_dependencies;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Settings::acceptInstallDependencies() const noexcept -> const Utils::BoolAspect & {
        return m_accept_install_dependencies;
    }
} // namespace XMakeProjectManager::Internal

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
} // namespace XMakeProjectManager::Internal

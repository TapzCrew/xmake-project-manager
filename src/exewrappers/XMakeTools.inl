#pragma once

#include "XMakeTools.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeTools::tools() noexcept -> std::span<const XMakeWrapperPtr> {
        return instance().m_tools;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeTools::instance() -> XMakeTools & {
        static auto instance = XMakeTools {};

        return instance;
    }
} // namespace XMakeProjectManager::Internal

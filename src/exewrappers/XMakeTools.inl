// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "XMakeTools.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeTools::tools() noexcept -> const std::vector<XMakeWrapperPtr> & {
        return instance().m_tools;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeTools::instance() -> XMakeTools & {
        static auto instance = XMakeTools {};

        return instance;
    }
} // namespace XMakeProjectManager::Internal

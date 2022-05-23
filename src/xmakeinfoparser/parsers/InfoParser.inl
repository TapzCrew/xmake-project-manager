// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "InfoParser.hpp"

namespace XMakeProjectManager::Internal {
    inline auto InfoParser::info() const noexcept -> const XMakeInfo & { return m_info; }
} // namespace XMakeProjectManager::Internal

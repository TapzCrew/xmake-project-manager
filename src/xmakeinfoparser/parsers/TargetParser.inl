// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "TargetParser.hpp"

namespace XMakeProjectManager::Internal {
    inline auto TargetParser::targets() const noexcept -> const TargetsList & { return m_targets; }
} // namespace XMakeProjectManager::Internal

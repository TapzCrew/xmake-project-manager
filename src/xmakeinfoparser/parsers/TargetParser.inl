#pragma once

#include "TargetParser.hpp"

namespace XMakeProjectManager::Internal {
    inline auto TargetParser::targets() const noexcept -> const TargetsList & { return m_targets; }
} // namespace XMakeProjectManager::Internal

#pragma once

#include "InfoParser.hpp"

namespace XMakeProjectManager::Internal {
    inline auto InfoParser::info() const noexcept -> const XMakeInfo & { return m_info; }
} // namespace XMakeProjectManager::Internal

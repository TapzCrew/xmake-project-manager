#pragma once

#include "BuildSystemFilesParser.hpp"

namespace XMakeProjectManager::Internal {
    inline auto BuildSystemFilesParser::files() const noexcept -> const std::vector<Utils::FilePath> & { return m_files; }
} // namespace XMakeProjectManager::Internal

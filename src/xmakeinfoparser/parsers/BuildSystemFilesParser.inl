// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto BuildSystemFilesParser::files() const noexcept
        -> const std::vector<Utils::FilePath> & {
        return m_files;
    }
} // namespace XMakeProjectManager::Internal

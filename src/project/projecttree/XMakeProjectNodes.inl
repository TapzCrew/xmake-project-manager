// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "XMakeProjectNodes.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeFileNode::showInSimpleTree() const -> bool { return false; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeFileNode::visibleAfterAddFileAction() const
        -> std::optional<Utils::FilePath> {
        return filePath().pathAppended("xmake.build");
    }
} // namespace XMakeProjectManager::Internal

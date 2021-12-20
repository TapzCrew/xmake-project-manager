#pragma once

#include "XMakeProjectNodes.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeFileNode::showInSimpleTree() const -> bool { return false; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeFileNode::visibleAfterAddFileAction() const
        -> Utils::optional<Utils::FilePath> {
        return filePath().pathAppended("xmake.build");
    }
} // namespace XMakeProjectManager::Internal

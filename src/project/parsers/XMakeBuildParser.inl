#pragma once

#include "XMakeBuildParser.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildParser::hasDetectedRedirection() const noexcept -> bool { return true; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildParser::hasFatalErrors() const noexcept -> bool {
        // TODO
        return false;
    }
} // namespace XMakeProjectManager::Internal

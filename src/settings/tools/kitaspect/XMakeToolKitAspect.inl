#pragma once

#include "XMakeToolKitAspect.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeToolKitAspect::xmakeTool(const ProjectExplorer::Kit *kit) -> decltype(auto) {
        return XMakeTools::xmakeWrapper(XMakeToolKitAspect::xmakeToolId(kit));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeToolKitAspect::isValid(const ProjectExplorer::Kit *kit) -> bool {
        auto tool = xmakeTool(kit);
        return (tool && tool->isValid());
    }
} // namespace XMakeProjectManager::Internal

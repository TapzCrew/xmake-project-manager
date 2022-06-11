// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

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

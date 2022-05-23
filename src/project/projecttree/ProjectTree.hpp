// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <xmakeinfoparser/XMakeInfoParser.hpp>

#include <project/projecttree/XMakeProjectNodes.hpp>

#include <utils/fileutils.h>

namespace XMakeProjectManager::Internal {
    class ProjectTree {
      public:
        ProjectTree();

        static std::unique_ptr<XMakeProjectNode>
            buildTree(const Utils::FilePath &src_dir,
                      const TargetsList &targets,
                      const std::vector<Utils::FilePath> &bs_files);
    };
} // namespace XMakeProjectManager::Internal

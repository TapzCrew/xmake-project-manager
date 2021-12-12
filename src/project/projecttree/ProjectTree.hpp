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

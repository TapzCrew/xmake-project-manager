#pragma once

#include <QString>

#include <vector>

namespace XMakeProjectManager::Internal {
    struct BuildOption {
        using OwnedPtr = std::unique_ptr<BuildOption>;
    };

    using BuildOptionsList = std::vector<BuildOption::OwnedPtr>;
} // namespace XMakeProjectManager::Internal

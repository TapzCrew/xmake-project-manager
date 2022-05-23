// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QString>

#include <vector>

namespace XMakeProjectManager::Internal {
    struct BuildOption {
        using OwnedPtr = std::unique_ptr<BuildOption>;
    };

    using BuildOptionsList = std::vector<BuildOption::OwnedPtr>;
} // namespace XMakeProjectManager::Internal

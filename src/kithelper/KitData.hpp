// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <utils/cpplanguage_details.h>

#include <QString>

namespace XMakeProjectManager::Internal {
    struct KitData {
        QString c_compiler_path;
        QString cxx_compiler_path;
    };
} // namespace XMakeProjectManager::Internal

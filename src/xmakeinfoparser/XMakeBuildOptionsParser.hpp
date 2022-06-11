// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QString>
#include <QStringList>

#include <memory>
#include <vector>

namespace XMakeProjectManager::Internal {
    struct BuildOption {
        QString name;
        QString description;
        QString value;
        QStringList values;

        QString xmakeArg() const noexcept;
    };

    using BuildOptionsList = std::vector<std::unique_ptr<BuildOption>>;
} // namespace XMakeProjectManager::Internal

#include "XMakeBuildOptionsParser.inl"

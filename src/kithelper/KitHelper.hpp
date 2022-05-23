// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <kithelper/KitData.hpp>

#include <projectexplorer/kit.h>
#include <projectexplorer/kitinformation.h>

#include <utils/macroexpander.h>
#include <utils/qtcassert.h>

namespace XMakeProjectManager::Internal::KitHelper {
    namespace details {
        QString expand(const ProjectExplorer::Kit *kit, const QString &macro);
    }

    QString cCompilerPath(const ProjectExplorer::Kit *kit);
    QString cxxCompilerPath(const ProjectExplorer::Kit *kit);
    QString cmakePath(const ProjectExplorer::Kit *kit);
    QString qmakePath(const ProjectExplorer::Kit *kit);
    QString qtVersion(const ProjectExplorer::Kit *kit);

    KitData kitData(const ProjectExplorer::Kit *kit);
} // namespace XMakeProjectManager::Internal::KitHelper

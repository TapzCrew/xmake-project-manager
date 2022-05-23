// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeTargetParser.hpp"

#include <utils/filepath.h>
#include <utils/fileutils.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto Target::fullName(const Utils::FilePath &src_dir,
                          const QString &target_file,
                          const QString &defined_in) -> QString {
        if (Utils::FileUtils::isAbsolutePath(target_file)) {
            const auto fname = target_file.split('/').last();

            auto definedIn = Utils::FilePath::fromString(defined_in).absolutePath().toString();

            return definedIn.remove(src_dir.toString()) + '/' + fname;
        } else
            return target_file;
    }
} // namespace XMakeProjectManager::Internal

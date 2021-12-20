#pragma once

#include <utils/cpplanguage_details.h>

#include <QString>

namespace XMakeProjectManager::Internal {
    struct KitData {
        QString c_compiler_path;
        QString cxx_compiler_path;
    };
} // namespace XMakeProjectManager::Internal

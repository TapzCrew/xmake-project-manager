#pragma once

#include <QString>

#include <vector>

namespace XMakeProjectManager::Internal {
    struct Target {
        enum class Kind {
            SHARED, BINARY, STATIC
        };

        QString name;
        Kind kind;
        QString defined_in;
    };

    using TargetsList = std::vector<Target>;
} // namespace XMakeProjectManager::Internal

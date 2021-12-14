#pragma once

#include <QString>
#include <QStringList>

#include <vector>

namespace XMakeProjectManager::Internal {
    struct Target {
        struct SourceGroup {
            QString language;
            QStringList sources;
        };
        using SourceGroupList = std::vector<SourceGroup>;

        enum class Kind { SHARED, BINARY, STATIC };

        QString name;
        Kind kind;
        QString defined_in;

        SourceGroupList sources;
        QStringList headers;
    };

    using TargetsList = std::vector<Target>;
} // namespace XMakeProjectManager::Internal

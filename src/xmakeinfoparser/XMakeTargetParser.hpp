#pragma once

#include <QString>
#include <QStringList>

#include <vector>

namespace Utils {
    class FilePath;
}

namespace XMakeProjectManager::Internal {
    struct Target {
        struct SourceGroup {
            QString language;
            QStringList sources;
            QStringList arguments;
        };
        using SourceGroupList = std::vector<SourceGroup>;

        enum class Kind { SHARED, BINARY, STATIC };

        QString name;
        Kind kind;
        QString defined_in;

        SourceGroupList sources;
        QStringList headers;

        QString target_file;

        static QString fullName(const Utils::FilePath &srcDir,
                                const QString &target_file,
                                const QString &defined_in);
    };

    using TargetsList = std::vector<Target>;
} // namespace XMakeProjectManager::Internal

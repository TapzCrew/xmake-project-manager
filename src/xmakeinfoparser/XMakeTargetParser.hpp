// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QString>
#include <QStringList>

#include <unordered_map>
#include <vector>

namespace Utils {
    class FilePath;
}

namespace XMakeProjectManager::Internal {
    struct Target {
        struct SourceGroup {
            QString kind;
            QStringList sources;
            QStringList arguments;
        };
        using SourceGroupList = std::vector<SourceGroup>;

        enum class Kind { SHARED, BINARY, STATIC, OBJECT, HEADERONLY, PHONY };

        QString name;
        Kind kind;
        QString defined_in;

        SourceGroupList sources;
        QStringList headers;
        QStringList modules;

        std::unordered_map<QString, QString> set_env;
        std::unordered_map<QString, QStringList> add_env;

        QString target_file;

        QStringList languages; // for MSVC code model

        QStringList group;

        QStringList packages;

        QStringList frameworks;

        bool use_qt;

        static QString fullName(const Utils::FilePath &srcDir,
                                const QString &target_file,
                                const QString &defined_in);
    };

    using TargetsList = std::vector<Target>;
} // namespace XMakeProjectManager::Internal

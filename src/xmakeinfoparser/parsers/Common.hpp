// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "utils/filepath.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QString>

#include <optional>

namespace XMakeProjectManager::Internal {
    template<typename T>
    T load(QJsonDocument &&doc);

    template<>
    inline QJsonArray load<QJsonArray>(QJsonDocument &&doc) {
        return doc.array();
    }

    template<>
    inline QJsonObject load<QJsonObject>(QJsonDocument &&doc) {
        return doc.object();
    }

    template<typename T>
    T load(const QJsonValueRef &ref);

    template<>
    inline QJsonArray load<QJsonArray>(const QJsonValueRef &ref) {
        return ref.toArray();
    }

    template<>
    inline QJsonObject load<QJsonObject>(const QJsonValueRef &ref) {
        return ref.toObject();
    }

    template<typename T>
    std::optional<T> get(const QJsonObject &obj, const QString &name);

    inline auto extractArray(const QJsonArray &array) -> QStringList {
        auto output = QStringList {};
        output.reserve(std::size(array));

        std::transform(std::cbegin(array),
                       std::cend(array),
                       std::back_inserter(output),
                       [](const auto &v) { return v.toString(); });

        return output;
    }

    inline auto extractPathArray(const QJsonArray &array, const Utils::FilePath &root)
        -> QStringList {
        auto output = QStringList {};
        output.reserve(std::size(array));

        std::transform(std::cbegin(array),
                       std::cend(array),
                       std::back_inserter(output),
                       [&root](const auto &v) {
                           auto path = Utils::FilePath::fromString(v.toString()).cleanPath();

                           if (!path.isAbsolutePath()) path = root.resolvePath(path);

                           return path.toString();
                       });

        return output;
    }

    template<>
    inline std::optional<QJsonArray> get<QJsonArray>(const QJsonObject &obj,
                                                       const QString &name) {
        if (obj.contains(name)) {
            auto child = obj[name];
            if (child.isArray()) return child.toArray();
        }
        return std::nullopt;
    }

    template<>
    inline std::optional<QJsonObject> get<QJsonObject>(const QJsonObject &obj,
                                                         const QString &name) {
        if (obj.contains(name)) {
            auto child = obj[name];
            if (child.isObject()) return child.toObject();
        }
        return std::nullopt;
    }

    template<typename T, typename... Strings>
    std::optional<T>
        get(const QJsonObject &obj, const QString &firstPath, const Strings &...path) {
        if (obj.contains(firstPath)) return get<T>(obj[firstPath].toObject(), path...);
        return std::nullopt;
    }
} // namespace XMakeProjectManager::Internal

// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValueRef>
#include <QString>

#include <utils/optional.h>

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
    Utils::optional<T> get(const QJsonObject &obj, const QString &name);

    template<>
    inline Utils::optional<QJsonArray> get<QJsonArray>(const QJsonObject &obj,
                                                       const QString &name) {
        if (obj.contains(name)) {
            auto child = obj[name];
            if (child.isArray()) return child.toArray();
        }
        return Utils::nullopt;
    }

    template<>
    inline Utils::optional<QJsonObject> get<QJsonObject>(const QJsonObject &obj,
                                                         const QString &name) {
        if (obj.contains(name)) {
            auto child = obj[name];
            if (child.isObject()) return child.toObject();
        }
        return Utils::nullopt;
    }

    template<typename T, typename... Strings>
    Utils::optional<T>
        get(const QJsonObject &obj, const QString &firstPath, const Strings &...path) {
        if (obj.contains(firstPath)) return get<T>(obj[firstPath].toObject(), path...);
        return Utils::nullopt;
    }
} // namespace XMakeProjectManager::Internal

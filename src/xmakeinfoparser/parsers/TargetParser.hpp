// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <vector>

#include <QString>
#include <QtGlobal>

#include <xmakeinfoparser/XMakeTargetParser.hpp>

QT_BEGIN_NAMESPACE
class QJsonDocument;
class QJsonObject;
class QJsonArray;
class QJsonValue;
QT_END_NAMESPACE

namespace XMakeProjectManager::Internal {
    class TargetParser {
      public:
        explicit TargetParser(const QJsonDocument &json);

        const TargetsList &targets() const noexcept;

      private:
        static TargetsList loadTargets(const QJsonArray &json_targets);
        static Target loadTarget(const QJsonValue &json_target);
        static Target::SourceGroupList extractSources(const QJsonArray &json_sources);
        static Target::SourceGroup extractSource(const QJsonValue &json_source);
        static QStringList extractHeaders(const QJsonArray &json_headers);
        static QStringList extractLanguages(const QJsonArray &json_languages);
        static QStringList extractPackages(const QJsonArray &json_packages);

        TargetsList m_targets;
    };
} // namespace XMakeProjectManager::Internal

#include "TargetParser.inl"

// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "utils/filepath.h"
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
        explicit TargetParser(const QJsonDocument &json, const Utils::FilePath &root);

        const TargetsList &targets() const noexcept;

      private:
        static TargetsList loadTargets(const QJsonArray &json_targets, const Utils::FilePath &root);
        static Target loadTarget(const QJsonValue &json_target, const Utils::FilePath &root);
        static Target::SourceGroupList extractSources(const QJsonArray &json_sources, const Utils::FilePath &root);
        static Target::SourceGroup extractSource(const QJsonValue &json_source, const Utils::FilePath &root);

        TargetsList m_targets;
    };
} // namespace XMakeProjectManager::Internal

#include "TargetParser.inl"

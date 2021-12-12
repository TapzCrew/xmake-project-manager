#pragma once

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

        TargetsList m_targets;
    };
} // namespace XMakeProjectManager::Internal

#include "TargetParser.inl"

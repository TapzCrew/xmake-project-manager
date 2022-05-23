// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QtGlobal>

#include <xmakeinfoparser/XMakeInfo.hpp>

QT_BEGIN_NAMESPACE
class QJsonDocument;
class QJsonObject;
QT_END_NAMESPACE

namespace XMakeProjectManager::Internal {
    class InfoParser {
      public:
        explicit InfoParser(const QJsonDocument &json);

        const XMakeInfo &info() const noexcept;

      private:
        static XMakeInfo loadInfo(const QJsonObject &obj);

        XMakeInfo m_info;
    };
} // namespace XMakeProjectManager::Internal

#include "InfoParser.inl"

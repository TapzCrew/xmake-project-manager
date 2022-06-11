// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QtGlobal>

#include <xmakeinfoparser/XMakeInfo.hpp>

#include <xmakeinfoparser/XMakeBuildOptionsParser.hpp>

QT_BEGIN_NAMESPACE
class QJsonDocument;
class QJsonObject;
class QJsonArray;
QT_END_NAMESPACE

namespace XMakeProjectManager::Internal {
    class OptionParser {
      public:
        explicit OptionParser(const QJsonDocument &json);

        BuildOptionsList options() const noexcept;

      private:
        static BuildOptionsList loadOptions(const QJsonArray &json_options);
        static std::unique_ptr<BuildOption> loadOption(const QJsonValue &json_option);

        BuildOptionsList m_options;
    };
} // namespace XMakeProjectManager::Internal

#include "OptionParser.inl"

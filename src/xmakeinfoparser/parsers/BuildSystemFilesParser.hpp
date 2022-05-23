// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QtGlobal>

#include <xmakeinfoparser/XMakeTargetParser.hpp>

#include <utils/fileutils.h>

QT_BEGIN_NAMESPACE
class QJsonDocument;
class QJsonObject;
class QJsonArray;
class QJsonValue;
QT_END_NAMESPACE

namespace XMakeProjectManager::Internal {
    class BuildSystemFilesParser {
      public:
        explicit BuildSystemFilesParser(const QJsonDocument &json);

        const std::vector<Utils::FilePath> &files() const noexcept;

      private:
        static std::vector<Utils::FilePath> loadFiles(const QJsonArray &json_targets);

        std::vector<Utils::FilePath> m_files;
    };
} // namespace XMakeProjectManager::Internal

#include "BuildSystemFilesParser.inl"

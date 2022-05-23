// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QStringList>

#include <xmakeinfoparser/XMakeInfo.hpp>
#include <xmakeinfoparser/XMakeTargetParser.hpp>

#include <utils/fileutils.h>
#include <utils/optional.h>

QT_BEGIN_NAMESPACE
class QByteArray;
QT_END_NAMESPACE

namespace XMakeProjectManager::Internal {
    class XmakeInfoParserPrivate;

    namespace XMakeInfoParser {
        struct Result {
            TargetsList targets;

            std::vector<Utils::FilePath> build_system_files;

            Utils::optional<XMakeInfo> xmake_info;
        };

        Result parse(const QByteArray &data);
    } // namespace XMakeInfoParser
} // namespace XMakeProjectManager::Internal

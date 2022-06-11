// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <xmakeinfoparser/XMakeInfoParser.hpp>

#include <xmakeinfoparser/parsers/BuildSystemFilesParser.hpp>
#include <xmakeinfoparser/parsers/InfoParser.hpp>
#include <xmakeinfoparser/parsers/OptionParser.hpp>
#include <xmakeinfoparser/parsers/TargetParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal::XMakeInfoParser {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto parse(const QByteArray &data) -> Result {
        auto json = QJsonDocument::fromJson(data);

        return { OptionParser { json }.options(),
                 TargetParser { json }.targets(),
                 BuildSystemFilesParser { json }.files(),
                 InfoParser { json }.info() };
    }
} // namespace XMakeProjectManager::Internal::XMakeInfoParser

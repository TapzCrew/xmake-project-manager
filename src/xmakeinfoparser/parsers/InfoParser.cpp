// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <xmakeinfoparser/parsers/Common.hpp>
#include <xmakeinfoparser/parsers/InfoParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    InfoParser::InfoParser(const QJsonDocument &json) { m_info = loadInfo(json.object()); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto InfoParser::loadInfo(const QJsonObject &obj) -> XMakeInfo {
        auto info = XMakeInfo {};

        info.xmake_version = Version::fromString(obj["version"].toString());

        return info;
    }
} // namespace XMakeProjectManager::Internal

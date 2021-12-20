#include <xmakeinfoparser/parsers/Common.hpp>
#include <xmakeinfoparser/parsers/InfoParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal {
    InfoParser::InfoParser(const QJsonDocument &json) { m_info = loadInfo(json.object()); }

    auto InfoParser::loadInfo(const QJsonObject &obj) -> XMakeInfo {
        auto info = XMakeInfo {};

        info.xmake_version = Version::fromString(obj["version"].toString());

        return info;
    }
} // namespace XMakeProjectManager::Internal

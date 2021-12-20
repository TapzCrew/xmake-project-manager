#include <xmakeinfoparser/XMakeInfoParser.hpp>

#include <xmakeinfoparser/parsers/InfoParser.hpp>
#include <xmakeinfoparser/parsers/TargetParser.hpp>
#include <xmakeinfoparser/parsers/BuildSystemFilesParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal::XMakeInfoParser {
    auto parse(const QByteArray &data) -> Result {
        auto json = QJsonDocument::fromJson(data);

        return { TargetParser { json }.targets(), BuildSystemFilesParser{json}.files(), InfoParser { json }.info() };
    }
} // namespace XMakeProjectManager::Internal::XMakeInfoParser

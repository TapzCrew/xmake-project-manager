#include <xmakeinfoparser/parsers/Common.hpp>

#include <xmakeinfoparser/parsers/TargetParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal {
    TargetParser::TargetParser(const QJsonDocument &json) {
        auto json_targets = get<QJsonArray>(json.object(), "targets");
        if (json_targets) m_targets = loadTargets(*json_targets);
    }

    auto TargetParser::loadTargets(const QJsonArray &json_targets) -> TargetsList {
        auto targets = TargetsList {};
        targets.reserve(json_targets.size());

        std::transform(std::cbegin(json_targets),
                       std::cend(json_targets),
                       std::back_inserter(targets),
                       loadTarget);

        return targets;
    }

    auto TargetParser::loadTarget(const QJsonValue &json_target) -> Target {
        auto json_target_obj = json_target.toObject();

        auto target = Target { json_target_obj["name"].toString() };

        auto kind = json_target_obj["kind"].toString();
        if(kind == "binary") target.kind = Target::Kind::BINARY;
        else if(kind == "shared") target.kind = Target::Kind::SHARED;
        else if(kind == "static") target.kind = Target::Kind::STATIC;

        target.defined_in = json_target["defined_in"].toString();

        return target;
    }
} // namespace XMakeProjectManager::Internal

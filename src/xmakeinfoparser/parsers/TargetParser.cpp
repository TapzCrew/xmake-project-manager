#include <xmakeinfoparser/parsers/Common.hpp>

#include <xmakeinfoparser/parsers/TargetParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal {
    TargetParser::TargetParser(const QJsonDocument &json) {
        auto json_targets = get<QJsonArray>(json.object(), "targets");
        if (json_targets) loadTargets(*json_targets);
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

        return target;
    }
} // namespace XMakeProjectManager::Internal

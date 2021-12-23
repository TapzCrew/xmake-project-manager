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
        if (kind == "binary") target.kind = Target::Kind::BINARY;
        else if (kind == "shared")
            target.kind = Target::Kind::SHARED;
        else if (kind == "static")
            target.kind = Target::Kind::STATIC;

        target.defined_in = json_target["defined_in"].toString();

        auto json_source_batches = json_target["source_batches"].toArray();
        target.sources           = extractSources(json_source_batches);

        auto json_headers = json_target["header_files"].toArray();
        target.headers    = extractHeaders(json_headers);

        target.target_file = json_target["target_file"].toString();

        return target;
    }

    auto TargetParser::extractSources(const QJsonArray &json_sources) -> Target::SourceGroupList {
        auto sources = Target::SourceGroupList {};
        sources.reserve(std::size(json_sources));

        std::transform(std::cbegin(json_sources),
                       std::cend(json_sources),
                       std::back_inserter(sources),
                       extractSource);

        return sources;
    }

    auto TargetParser::extractSource(const QJsonValue &json_source) -> Target::SourceGroup {
        const auto source = json_source.toObject();

        return { json_source["kind"].toString(),
                 json_source["source_files"].toVariant().toStringList(),
                 json_source["arguments"].toVariant().toStringList() };
    }

    auto TargetParser::extractHeaders(const QJsonArray &json_headers) -> QStringList {
        auto headers = QStringList {};
        headers.reserve(std::size(json_headers));

        std::transform(std::cbegin(json_headers),
                       std::cend(json_headers),
                       std::back_inserter(headers),
                       [](const auto &v) { return v.toString(); });

        return headers;
    }
} // namespace XMakeProjectManager::Internal

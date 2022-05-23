// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <xmakeinfoparser/parsers/Common.hpp>

#include <xmakeinfoparser/parsers/TargetParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto extractArray(const QJsonArray &array) -> QStringList {
        auto output = QStringList {};
        output.reserve(std::size(array));

        std::transform(std::cbegin(array),
                       std::cend(array),
                       std::back_inserter(output),
                       [](const auto &v) { return v.toString(); });

        return output;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    TargetParser::TargetParser(const QJsonDocument &json) {
        auto json_targets = get<QJsonArray>(json.object(), "targets");
        if (json_targets) m_targets = loadTargets(*json_targets);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto TargetParser::loadTargets(const QJsonArray &json_targets) -> TargetsList {
        auto targets = TargetsList {};
        targets.reserve(json_targets.size());

        std::transform(std::cbegin(json_targets),
                       std::cend(json_targets),
                       std::back_inserter(targets),
                       loadTarget);

        return targets;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
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
        target.headers    = extractArray(json_headers);

        target.target_file = json_target["target_file"].toString();

        auto json_languages = json_target["languages"].toArray();
        target.languages    = extractArray(json_languages);

        auto group   = json_target["group"].toString();
        target.group = group.split('/');

        auto json_packages = json_target["packages"].toArray();
        target.packages    = extractArray(json_packages);

        auto json_frameworks = json_target["frameworks"].toArray();
        target.frameworks    = extractArray(json_frameworks);

        return target;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto TargetParser::extractSources(const QJsonArray &json_sources) -> Target::SourceGroupList {
        auto sources = Target::SourceGroupList {};
        sources.reserve(std::size(json_sources));

        std::transform(std::cbegin(json_sources),
                       std::cend(json_sources),
                       std::back_inserter(sources),
                       extractSource);

        return sources;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto TargetParser::extractSource(const QJsonValue &json_source) -> Target::SourceGroup {
        const auto source = json_source.toObject();

        return { source["kind"].toString(),
                 source["source_files"].toVariant().toStringList(),
                 source["arguments"].toVariant().toStringList() };
    }
} // namespace XMakeProjectManager::Internal

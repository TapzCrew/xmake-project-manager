// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <utils/filepath.h>
#include <xmakeinfoparser/parsers/Common.hpp>

#include <xmakeinfoparser/parsers/TargetParser.hpp>

#include <QJsonDocument>
#include <QJsonValue>
#include <QMapIterator>

namespace XMakeProjectManager::Internal {

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    TargetParser::TargetParser(const QJsonDocument &json, const Utils::FilePath &root) {
        auto json_targets = get<QJsonArray>(json.object(), "targets");

        if (json_targets) m_targets = loadTargets(*json_targets, root);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto TargetParser::loadTargets(const QJsonArray &json_targets, const Utils::FilePath &root)
        -> TargetsList {
        auto targets = TargetsList {};
        targets.reserve(json_targets.size());

        std::transform(std::cbegin(json_targets),
                       std::cend(json_targets),
                       std::back_inserter(targets),
                       [&root](const auto &v) { return loadTarget(v, root); });

        std::sort(targets.begin(), targets.end(), [](const auto &a, const auto &b) {
            return a.name.compare(b.name);
        });

        targets.erase(std::unique(targets.begin(),
                                  targets.end(),
                                  [](const auto &a, const auto &b) { return a.name == b.name; }),
                      targets.end());

        return targets;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto TargetParser::loadTarget(const QJsonValue &json_target, const Utils::FilePath &root)
        -> Target {
        auto json_target_obj = json_target.toObject();

        auto target = Target { json_target_obj["name"].toString() };

        auto kind = json_target_obj["kind"].toString();
        if (kind == "binary") target.kind = Target::Kind::BINARY;
        else if (kind == "shared")
            target.kind = Target::Kind::SHARED;
        else if (kind == "static")
            target.kind = Target::Kind::STATIC;

        target.defined_in = [&] {
            auto path =
                Utils::FilePath::fromString(json_target["defined_in"].toString()).cleanPath();
            if (!path.isAbsolutePath()) path = root.resolvePath(path);

            return path.toString();
        }();

        auto json_source_batches = json_target["source_batches"].toArray();
        target.sources           = extractSources(json_source_batches, root);

        auto json_headers = json_target["header_files"].toArray();
        target.headers    = extractPathArray(json_headers, root);
        target.headers.removeDuplicates();

        auto json_modules = json_target["module_files"].toArray();
        target.modules    = extractPathArray(json_modules, root);
        target.modules.removeDuplicates();

        auto json_run_envs = json_target["run_envs"];
        auto set_run_envs  = json_run_envs["set"].toObject().toVariantMap();

        for (auto it = set_run_envs.begin(); it != set_run_envs.end(); ++it) {
            target.set_env.emplace(it.key(), it.value().toString());
        }

        auto add_run_envs = json_run_envs["add"].toObject().toVariantMap();

        for (auto it = add_run_envs.begin(); it != add_run_envs.end(); ++it) {
            const auto values = it.value().toJsonArray();

            auto &output = target.add_env.emplace(it.key(), QStringList {}).first->second;

            for (const auto &value : values) { output.emplace_back(value.toString()); }
        }

        target.target_file = [&] {
            auto path =
                Utils::FilePath::fromString(json_target["target_file"].toString()).cleanPath();

            if (!path.isAbsolutePath()) path = root.resolvePath(path);

            return path.toString();
        }();

        auto json_languages = json_target["languages"].toArray();
        target.languages    = extractArray(json_languages);
        target.languages.removeDuplicates();

        auto group   = json_target["group"].toString();
        target.group = group.split('/');

        auto json_packages = json_target["packages"].toArray();
        target.packages    = extractArray(json_packages);
        target.packages.removeDuplicates();

        auto json_frameworks = json_target["frameworks"].toArray();
        target.frameworks    = extractArray(json_frameworks);
        target.frameworks.removeDuplicates();

        target.use_qt = json_target["use_qt"].toBool();

        return target;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto TargetParser::extractSources(const QJsonArray &json_sources, const Utils::FilePath &root)
        -> Target::SourceGroupList {
        auto sources = Target::SourceGroupList {};
        sources.reserve(std::size(json_sources));

        std::transform(std::cbegin(json_sources),
                       std::cend(json_sources),
                       std::back_inserter(sources),
                       [&root](const auto &v) { return extractSource(v, root); });

        return sources;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto TargetParser::extractSource(const QJsonValue &json_source, const Utils::FilePath &root)
        -> Target::SourceGroup {
        auto output =
            Target::SourceGroup { json_source["kind"].toString(),
                                  extractPathArray(json_source["source_files"].toArray(), root),
                                  extractArray(json_source["arguments"].toArray()) };

        output.sources.removeDuplicates();
        output.arguments.removeDuplicates();

        return output;
    }
} // namespace XMakeProjectManager::Internal

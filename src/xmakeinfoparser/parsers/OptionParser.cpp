// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <xmakeinfoparser/XMakeBuildOptionsParser.hpp>
#include <xmakeinfoparser/parsers/Common.hpp>
#include <xmakeinfoparser/parsers/OptionParser.hpp>

#include <QJsonArray>
#include <QJsonDocument>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    OptionParser::OptionParser(const QJsonDocument &json) {
        auto json_options = get<QJsonArray>(json.object(), "options");
        if (json_options) m_options = loadOptions(*json_options);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto OptionParser::loadOptions(const QJsonArray &json_options) -> BuildOptionsList {
        auto options = BuildOptionsList {};
        options.reserve(json_options.size());

        std::transform(std::cbegin(json_options),
                       std::cend(json_options),
                       std::back_inserter(options),
                       loadOption);

        return options;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto OptionParser::loadOption(const QJsonValue &json_option) -> std::unique_ptr<BuildOption> {
        auto _option = std::make_unique<BuildOption>();
        auto &option = *_option;

        option.name        = json_option["name"].toString();
        option.description = json_option["description"].toString();
        option.value       = json_option["value"].toString();

        auto json_values = json_option["values"].toArray();
        option.values    = extractArray(json_values);

        return _option;
    }

} // namespace XMakeProjectManager::Internal

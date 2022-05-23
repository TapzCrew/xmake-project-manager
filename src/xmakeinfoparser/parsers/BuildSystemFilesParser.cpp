// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <xmakeinfoparser/parsers/Common.hpp>

#include <xmakeinfoparser/parsers/BuildSystemFilesParser.hpp>

#include <QJsonDocument>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    BuildSystemFilesParser::BuildSystemFilesParser(const QJsonDocument &json) {
        auto json_files = get<QJsonArray>(json.object(), "project_files");
        if (json_files) m_files = loadFiles(*json_files);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildSystemFilesParser::loadFiles(const QJsonArray &json_files)
        -> std::vector<Utils::FilePath> {
        auto files = std::vector<Utils::FilePath> {};
        files.reserve(json_files.size());

        std::transform(std::cbegin(json_files),
                       std::cend(json_files),
                       std::back_inserter(files),
                       [](const auto &file) {
                           return Utils::FilePath::fromString(file.toString());
                       });

        return files;
    }
} // namespace XMakeProjectManager::Internal

// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <iterator>
#include <xmakeinfoparser/parsers/Common.hpp>

#include <xmakeinfoparser/parsers/BuildSystemFilesParser.hpp>

#include <QJsonDocument>

#include <utils/filepath.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    BuildSystemFilesParser::BuildSystemFilesParser(const QJsonDocument &json,
                                                   const Utils::FilePath &root) {
        auto json_files = get<QJsonArray>(json.object(), "project_files");

        if (json_files) {
            auto files = extractPathArray(*json_files, root);
            files.removeDuplicates();

            m_files.reserve(std::size(files));
            std::transform(std::cbegin(files),
                           std::cend(files),
                           std::back_inserter(m_files),
                           Utils::FilePath::fromString);
        }
    }
} // namespace XMakeProjectManager::Internal

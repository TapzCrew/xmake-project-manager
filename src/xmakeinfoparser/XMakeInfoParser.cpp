// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <xmakeinfoparser/XMakeInfoParser.hpp>

#include <xmakeinfoparser/parsers/BuildSystemFilesParser.hpp>
#include <xmakeinfoparser/parsers/Common.hpp>
#include <xmakeinfoparser/parsers/InfoParser.hpp>
#include <xmakeinfoparser/parsers/OptionParser.hpp>
#include <xmakeinfoparser/parsers/TargetParser.hpp>

#include <QJsonArray>
#include <QJsonDocument>

namespace XMakeProjectManager::Internal::XMakeInfoParser {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto parse(const QByteArray &data) -> Result {
        auto json = QJsonDocument::fromJson(data);

        const auto json_qml_import_paths = json["qml_import_path"].toArray();
        const auto project_dir =
            Utils::FilePath::fromString(json["project_dir"].toString()).cleanPath();

        return { OptionParser { json }.options(),
                 TargetParser { json, project_dir }.targets(),
                 project_dir,
                 BuildSystemFilesParser { json, project_dir }.files(),
                 extractPathArray(json_qml_import_paths, project_dir),
                 InfoParser { json }.info() };
    }
} // namespace XMakeProjectManager::Internal::XMakeInfoParser

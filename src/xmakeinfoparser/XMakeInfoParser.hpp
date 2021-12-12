#pragma once

#include <QStringView>

#include <xmakeinfoparser/XMakeInfo.hpp>
#include <xmakeinfoparser/XMakeTargetParser.hpp>

#include <utils/fileutils.h>
#include <utils/optional.h>

namespace XMakeProjectManager::Internal {
    class XmakeInfoParserPrivate;

    namespace XMakeInfoParser {
        struct Result {
            TargetsList targets;

            std::vector<Utils::FilePath> build_system_files;

            Utils::optional<XMakeInfo> xmake_info;
        };

        Result parse(const QByteArray &data);
    } // namespace XMakeInfoParser
} // namespace XMakeProjectManager::Internal

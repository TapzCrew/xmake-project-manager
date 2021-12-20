#include "XMakeBuildParser.hpp"

#include <utils/fileutils.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildParser::XMakeBuildParser() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildParser::~XMakeBuildParser() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildParser::handleLine(const QString &line, Utils::OutputFormat type) -> Result {
        if (type == Utils::OutputFormat::StdOutFormat) {
            auto progress = extractProgress(line);

            if (progress) {
                Q_EMIT reportProgress(*progress);

                return ProjectExplorer::OutputTaskParser::Status::InProgress;
            }
        }

        return ProjectExplorer::OutputTaskParser::Status::NotHandled;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildParser::setSourceDirectory(const Utils::FilePath &source_dir) -> void {
        Q_EMIT newSearchDirFound(source_dir);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildParser::extractProgress(QStringView line) -> Utils::optional<int> {
        auto progress = m_progress_regex.match(line);

        if (progress.hasMatch()) return progress.captured(1).toInt();

        return Utils::nullopt;
    }
} // namespace XMakeProjectManager::Internal

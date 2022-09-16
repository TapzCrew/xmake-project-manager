// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeBuildParser.hpp"

#include <utils/fileutils.h>

#include <projectexplorer/taskhub.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildParser::XMakeBuildParser(Type type) : m_has_char_number { type != Type::MSVC } {
        m_error_regex =
            QRegularExpression { (type == Type::MSVC) ? m_msvc_error_regex : m_gcc_error_regex };
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildParser::~XMakeBuildParser() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildParser::handleLine(const QString &line, Utils::OutputFormat type) -> Result {
        if (type != Utils::OutputFormat::StdOutFormat)
            return ProjectExplorer::OutputTaskParser::Status::NotHandled;

        auto progress = extractProgress(line);

        if (progress) {
            Q_EMIT reportProgress(*progress);

            return ProjectExplorer::OutputTaskParser::Status::InProgress;
        }

        auto error = m_error_regex.match(line);
        if (error.hasMatch()) {
            auto link_specs = addTask(ProjectExplorer::Task::TaskType::Error, error, 1, 2, 3, 4);
            return { ProjectExplorer::OutputTaskParser::Status::Done, link_specs };
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
    auto XMakeBuildParser::extractProgress(QStringView line) -> std::optional<int> {
        auto progress = m_progress_regex.match(line);

        if (progress.hasMatch()) return progress.captured(1).toInt();

        return std::nullopt;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildParser::addTask(ProjectExplorer::Task::TaskType type,
                                   const QRegularExpressionMatch &match,
                                   int file_cap_index,
                                   int line_number_cap_index,
                                   int char_number_cap_index,
                                   int error_cap_index) -> LinkSpecs {
        auto link_specs = LinkSpecs {};

        auto file_name =
            absoluteFilePath(Utils::FilePath::fromString(match.captured(file_cap_index)));
        auto task = ProjectExplorer::CompileTask {
            type,
            QString { QStringLiteral("%1") }.arg(
                match.captured((m_has_char_number) ? error_cap_index : char_number_cap_index)),
            file_name,
            match.captured(line_number_cap_index).toInt(),
            (m_has_char_number) ? match.captured(char_number_cap_index).toInt() : 0
        };

        addLinkSpecForAbsoluteFilePath(link_specs, task.file, task.line, match, file_cap_index);
        ProjectExplorer::TaskHub::addTask(std::move(task));

        return link_specs;
    }
} // namespace XMakeProjectManager::Internal

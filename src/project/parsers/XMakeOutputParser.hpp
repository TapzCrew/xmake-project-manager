// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <projectexplorer/ioutputparser.h>

#include <QRegularExpression>

namespace XMakeProjectManager::Internal {
    class XMakeOutputParser final: public ProjectExplorer::OutputTaskParser {
        Q_OBJECT
      public:
        XMakeOutputParser(bool capture_stdio = false);
        ~XMakeOutputParser();

        XMakeOutputParser(XMakeOutputParser &&)      = delete;
        XMakeOutputParser(const XMakeOutputParser &) = delete;

        XMakeOutputParser &operator=(XMakeOutputParser &&)             = delete;
        XMakeOutputParser &operator=(const XMakeOutputParser &&) const = delete;

        Result handleLine(const QString &line, Utils::OutputFormat type) override;

        void setSourceDirectory(const Utils::FilePath &source_dir);

        const QString &data() const noexcept;

      Q_SIGNALS:
        void new_search_dir_found(const Utils::FilePath &);

      private:
        void pushLine(const QString &line);

        Result processErrors(QStringView line);
        Result processWarnings(QStringView line);

        void addTask(ProjectExplorer::Task::TaskType type, QStringView line);
        void addTask(ProjectExplorer::Task task);
        LinkSpecs addTask(ProjectExplorer::Task::TaskType type,
                          QStringView line,
                          const QRegularExpressionMatch &match,
                          QStringView output_type,
                          int file_cap_index,
                          int line_number_cap_index,
                          int error_cap_index);

        const QRegularExpression m_error_file_location_regex { R"|(error:(.*) (.*):(\d+): (.*))|" };
        const QRegularExpression m_options_errors_regex { R"(error: (.*))" };
        const QRegularExpression m_warnings_regex { R"(warning: (.*))" };
        const QRegularExpression m_warning_file_location_regex { R"(warning: (.*) at (.*):(\d+))" };

        int m_remaining_lines = 0;

        QStringList m_pending_lines;

        bool m_capture_stdio;
        QString m_data;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeOutputParser.inl"

// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <projectexplorer/ioutputparser.h>

#include <QRegularExpression>

namespace XMakeProjectManager::Internal {
    class XMakeBuildParser final: public ProjectExplorer::OutputTaskParser {
        Q_OBJECT
      public:
        XMakeBuildParser();
        ~XMakeBuildParser();

        XMakeBuildParser(XMakeBuildParser &&)      = delete;
        XMakeBuildParser(const XMakeBuildParser &) = delete;

        XMakeBuildParser &operator=(XMakeBuildParser &&) = delete;
        XMakeBuildParser &operator=(const XMakeBuildParser &&) const = delete;

        Result handleLine(const QString &line, Utils::OutputFormat type) override;
        void setSourceDirectory(const Utils::FilePath &source_dir);

        bool hasDetectedRedirection() const noexcept override;

        bool hasFatalErrors() const noexcept override;

      Q_SIGNALS:
        void reportProgress(int progress);

      private:
        Utils::optional<int> extractProgress(QStringView line);
        LinkSpecs addTask(ProjectExplorer::Task::TaskType type,
                          const QRegularExpressionMatch &match,
                          int file_cap_index,
                          int line_number_cap_index,
                          int char_number_cap_index,
                          int error_cap_index);

        // error: test/main.cpp:12:3: error: ‘a’ was not declared in this scope
        const QRegularExpression m_progress_regex { R"(^\[\s*(\d+)\%\])" };
        const QRegularExpression m_error_regex { R"(error: (.*):(\d+):(\d+): error: (.*))" };
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeBuildParser.inl"

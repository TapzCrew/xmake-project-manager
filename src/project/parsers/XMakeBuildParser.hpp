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

        const QRegularExpression m_progress_regex { R"(^\[\s*(\d+)\%\])" };
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeBuildParser.inl"

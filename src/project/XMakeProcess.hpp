// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "utils/filepath.h"
#include <exewrappers/XMakeWrapper.hpp>

#include <utils/outputformatter.h>

#include <QElapsedTimer>
#include <QFutureInterface>
#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
template<class T>
class QFutureWatcher;
QT_END_NAMESPACE

namespace Utils {
    class QtcProcess;
    class Environment;
} // namespace Utils

namespace XMakeProjectManager::Internal {
    class XMakeOutputParser;
    class XMakeProcess final: public QObject {
        Q_OBJECT

      public:
        XMakeProcess();
        ~XMakeProcess();

        void run(const Command &command,
                 const Utils::Environment &env,
                 const QString &project_name,
                 const Utils::FilePath &source_path,
                 bool capture_stdio);
        void stop();

        [[nodiscard]] int lastExitCode() const noexcept;

      Q_SIGNALS:
        void started();
        void finished(int, QString);

      private:
        void handleProcessDone(const Utils::ProcessResultData &result_data);
        void
            setupProcess(const Command &command, const Utils::Environment &env, bool capture_stdio);

        bool sanityCheck(const Command &command) const;

        std::unique_ptr<Utils::QtcProcess> m_process;
        QFutureInterface<void> m_future_interface;
        std::unique_ptr<QFutureWatcher<void>> m_future_watcher;

        Utils::OutputFormatter m_parser;
        QElapsedTimer m_elapsed;
        int m_last_exit_code = 0;

        XMakeOutputParser *m_output_parser;

        QString m_data;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeProcess.inl"

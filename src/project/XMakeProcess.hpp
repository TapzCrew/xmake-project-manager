#pragma once

#include <exewrappers/XMakeWrapper.hpp>

#include <utils/qtcprocess.h>

#include <QByteArray>
#include <QElapsedTimer>
#include <QFutureInterface>
#include <QProcess>
#include <QTimer>

#include <memory>

namespace XMakeProjectManager::Internal {
    class XMakeProcess final: public QObject {
        Q_OBJECT

      public:
        XMakeProcess();

        bool run(const Command &command,
                 const Utils::Environment &env,
                 const QString &project_name,
                 bool capture_stdo = true);

        QProcess::ProcessState state() const;

        void reportCanceled();
        void reportFinished();
        void setProgressValue(int p);

        const QByteArray &stdOut() const noexcept;

        const Command &currentCommand() const noexcept;

      Q_SIGNALS:
        void started();
        void finished(int exit_code, QProcess::ExitStatus exit_status);
        void readyReadStandardOutput(const QByteArray &data);

      private:
        void handleProcessFinished(int code, QProcess::ExitStatus status);
        void handleProcessError(QProcess::ProcessError error);
        void checkForCancelled();
        void setupProcess(const Command &command, const Utils::Environment &env, bool capture_stdo);

        bool sanityCheck(const Command &command) const;

        void processStandardOutput();
        void processStandardError();

        std::unique_ptr<Utils::QtcProcess> m_process;
        QFutureInterface<void> m_future;
        bool m_process_was_canceled = false;
        QTimer m_cancel_timer;
        QElapsedTimer m_elapsed;
        QByteArray m_stdo;
        Command m_current_command;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeProcess.inl"

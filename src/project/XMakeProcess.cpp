// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "projectexplorer/task.h"
#include <project/XMakeProcess.hpp>

#include <project/parsers/XMakeBuildParser.hpp>

#include <coreplugin/messagemanager.h>
#include <coreplugin/progressmanager/progressmanager.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/taskhub.h>

#include <utils/stringutils.h>

#include <QLoggingCategory>

namespace XMakeProjectManager::Internal {
    static constexpr auto CANCEL_TIMER_INTERVAL         = 500;
    static constexpr auto CONFIGURE_TASK_ESTIMATED_TIME = 10;

    static Q_LOGGING_CATEGORY(xmakeProcessLog, "qtc.xmake.buildsystem", QtDebugMsg);

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProcess::XMakeProcess() {
        connect(&m_cancel_timer, &QTimer::timeout, this, &XMakeProcess::checkForCancelled);

        m_cancel_timer.setInterval(CANCEL_TIMER_INTERVAL);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::run(const Command &command,
                           const Utils::Environment &env,
                           const QString &project_name,
                           bool capture_stdo) -> bool {
        if (!sanityCheck(command)) return false;

        m_current_command = command;
        m_stdo.clear();
        m_process_was_canceled = false;
        m_future               = QFutureInterface<void> {};

        ProjectExplorer::TaskHub::clearTasks(
            static_cast<const char *>(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        setupProcess(command, env, capture_stdo);

        m_future.setProgressRange(0, 1);

        Core::ProgressManager::addTimedTask(m_future,
                                            tr("Configuring \"%1\".").arg(project_name),
                                            "XMake.Configure",
                                            CONFIGURE_TASK_ESTIMATED_TIME);

        Q_EMIT started();

        m_elapsed.start();
        m_process->start();
        m_cancel_timer.start(CANCEL_TIMER_INTERVAL);

        qCDebug(xmakeProcessLog()) << "Starting: " << command.toUserOutput();

        return true;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::handleProcessDone() -> void {
        if (m_process->result() != Utils::ProcessResult::FinishedWithSuccess) {
            ProjectExplorer::TaskHub::addTask(
                ProjectExplorer::BuildSystemTask { ProjectExplorer::Task::TaskType::Error,
                                                   m_process->exitMessage() });
        }

        m_cancel_timer.stop();

        m_stdo = m_process->readAllStandardOutput();

        if (m_process->exitStatus() == QProcess::NormalExit) {
            m_future.setProgressValue(1);
            m_future.reportFinished();
        } else {
            m_future.reportCanceled();
            m_future.reportFinished();
        }

        const auto elapsed_time = Utils::formatElapsedTime(m_elapsed.elapsed());

        Core::MessageManager::writeSilently(elapsed_time);

        Q_EMIT finished(m_process->exitCode(), m_process->exitStatus());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::checkForCancelled() -> void {
        if (m_future.isCanceled()) {
            m_cancel_timer.stop();
            m_process_was_canceled = true;
            m_process->close();
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::setupProcess(const Command &command,
                                    const Utils::Environment &env,
                                    bool capture_stdo) -> void {
        if (m_process) { m_process->interrupt(); }

        m_process = std::make_unique<Utils::QtcProcess>();
        connect(m_process.get(), &Utils::QtcProcess::done, this, &XMakeProcess::handleProcessDone);

        if (!capture_stdo) {
            connect(m_process.get(),
                    &Utils::QtcProcess::readyReadStandardOutput,
                    this,
                    &XMakeProcess::processStandardOutput);
            connect(m_process.get(),
                    &Utils::QtcProcess::readyReadStandardError,
                    this,
                    &XMakeProcess::processStandardError);
        }

        m_process->setWorkingDirectory(command.workDir());
        m_process->setEnvironment(env);

        Core::MessageManager::writeFlashing(
            tr("Running %1 in %2.").arg(command.toUserOutput(), command.workDir().toUserOutput()));

        m_process->setCommand(command.cmdLine());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::sanityCheck(const Command &command) const -> bool {
        const auto &exe = command.cmdLine().executable();

        if (!exe.exists()) {
            ProjectExplorer::TaskHub::addTask(ProjectExplorer::BuildSystemTask(
                ProjectExplorer::Task::TaskType::Error,
                tr("Executable does not exist: %1").arg(exe.toUserOutput())));

            return false;
        }

        if (!exe.toFileInfo().isExecutable()) {
            ProjectExplorer::TaskHub::addTask(ProjectExplorer::BuildSystemTask {
                ProjectExplorer::Task::TaskType::Error,
                tr("Command is not executable: %1").arg(exe.toUserOutput()) });

            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::processStandardOutput() -> void {
        const auto data = m_process->readAllStandardOutput();

        Core::MessageManager::writeSilently(QString::fromLocal8Bit(data));

        Q_EMIT readyReadStandardOutput(data);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::processStandardError() -> void {
        const auto data = m_process->readAllStandardError();

        Core::MessageManager::writeSilently(QString::fromLocal8Bit(data));
    }
} // namespace XMakeProjectManager::Internal

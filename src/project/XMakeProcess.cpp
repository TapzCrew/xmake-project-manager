// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "project/parsers/XMakeOutputParser.hpp"
#include "projectexplorer/task.h"
#include <project/XMakeProcess.hpp>

#include <project/parsers/XMakeBuildParser.hpp>

#include <coreplugin/progressmanager/progressmanager.h>

#include <projectexplorer/buildsystem.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/taskhub.h>

#include <utils/outputformat.h>
#include <utils/processinterface.h>
#include <utils/stringutils.h>

#include <QFutureInterface>
#include <QFutureWatcher>
#include <QLoggingCategory>

namespace XMakeProjectManager::Internal {
    static constexpr auto CANCEL_TIMER_INTERVAL         = 500;
    static constexpr auto CONFIGURE_TASK_ESTIMATED_TIME = 10;

    static QString stripTrailingNewline(QStringView str) {
        if (str.endsWith('\n')) str.chop(1);
        return str.toString();
    }

    static Q_LOGGING_CATEGORY(xmakeProcessLog, "qtc.xmake.buildsystem", QtDebugMsg);

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProcess::XMakeProcess() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProcess::~XMakeProcess() {
        m_parser.flush();

        if (m_future_watcher) {
            m_future_watcher.reset();

            m_future_interface.reportCanceled();
            m_future_interface.reportFinished();
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::run(const Command &command,
                           const Utils::Environment &env,
                           const QString &project_name,
                           const Utils::FilePath &source_path,
                           bool capture_stdio) -> void {
        QTC_ASSERT(!m_process, return );
        if (!sanityCheck(command)) return;

        m_output_parser = new XMakeOutputParser { capture_stdio };
        m_output_parser->setSourceDirectory(source_path);
        m_parser.addLineParser(m_output_parser);

        setupProcess(command, env, capture_stdio);

        ProjectExplorer::TaskHub::clearTasks(
            static_cast<const char *>(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));

        ProjectExplorer::BuildSystem::startNewBuildSystemOutput(
            tr("Running %1 in %2.").arg(command.toUserOutput(), command.workDir().toUserOutput()));

        m_future_interface = QFutureInterface<void>();
        m_future_interface.setProgressRange(0, 1);

        Core::ProgressManager::addTimedTask(m_future_interface,
                                            tr("Configuring \"%1\".").arg(project_name),
                                            "XMake.Configure",
                                            CONFIGURE_TASK_ESTIMATED_TIME);

        m_future_watcher = std::make_unique<QFutureWatcher<void>>();
        connect(m_future_watcher.get(), &QFutureWatcher<void>::canceled, this, &XMakeProcess::stop);
        m_future_watcher->setFuture(m_future_interface.future());

        Q_EMIT started();

        m_elapsed.start();
        m_process->start();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::stop() -> void {
        if (!m_process) return;

        m_process->close();

        handleProcessDone({ -1, QProcess::CrashExit, QProcess::Crashed, {} });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::handleProcessDone(const Utils::ProcessResultData &result_data) -> void {
        if (m_future_watcher) {
            m_future_watcher->disconnect();
            m_future_watcher.release()->deleteLater();
        }

        const auto code  = result_data.m_exitCode;
        const auto error = result_data.m_error;

        auto message = QString {};
        switch (error) {
            case QProcess::FailedToStart: message = tr("XMake process failed to start."); break;
            case QProcess::Timedout: message = tr("XMake process timedout."); break;
            case QProcess::ReadError: message = tr("XMake process read error."); break;
            case QProcess::WriteError: message = tr("XMake process write error."); break;
            default: {
                if (result_data.m_exitStatus != QProcess::NormalExit) {
                    if (m_future_interface.isCanceled() || code != 0)
                        message = tr("XMake process was canceled by the user.");
                } else if (code != 0)
                    message = tr("XMake process was exited with code %1").arg(code);
            } break;
        }

        m_last_exit_code = code;

        if (!message.isEmpty()) {
            ProjectExplorer::BuildSystem::appendBuildSystemOutput(message + '\n');
            ProjectExplorer::TaskHub::addTask(
                ProjectExplorer::BuildSystemTask { ProjectExplorer::Task::Error, message });
            m_future_interface.reportCanceled();
        } else
            m_future_interface.setProgressValue(1);

        m_future_interface.reportFinished();

        auto std_out = m_output_parser->data();

        Q_EMIT finished(code, std_out);

        auto elapsed_time = Utils::formatElapsedTime(m_elapsed.elapsed());
        ProjectExplorer::BuildSystem::appendBuildSystemOutput(elapsed_time + '\n');
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::setupProcess(const Command &command,
                                    const Utils::Environment &env,
                                    bool capture_stdio) -> void {
        m_process = std::make_unique<Utils::QtcProcess>();
        m_process->setWorkingDirectory(command.workDir());
        m_process->setEnvironment(env);

        m_process->setStdOutLineCallback([&](const QString &s) {
            m_parser.appendMessage(s, Utils::StdOutFormat);

            if (!capture_stdio)
                ProjectExplorer::BuildSystem::appendBuildSystemOutput(stripTrailingNewline(s));
        });

        m_process->setStdErrLineCallback([this](const QString &s) {
            m_parser.appendMessage(s, Utils::StdErrFormat);
            ProjectExplorer::BuildSystem::appendBuildSystemOutput(stripTrailingNewline(s));
        });

        connect(m_process.get(), &Utils::QtcProcess::done, this, [this] {
            handleProcessDone(m_process->resultData());
        });

        ProjectExplorer::TaskHub::clearTasks(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM);

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
} // namespace XMakeProjectManager::Internal

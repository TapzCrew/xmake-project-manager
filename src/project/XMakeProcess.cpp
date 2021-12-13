#include <project/XMakeProcess.hpp>

#include <project/parsers/XMakeBuildParser.hpp>

#include <coreplugin/messagemanager.h>
#include <coreplugin/progressmanager/progressmanager.h>

#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/taskhub.h>

#include <utils/stringutils.h>

#include <QLoggingCategory>

namespace XMakeProjectManager::Internal {
    static constexpr auto CANCEL_TIMER_INTERVAL = 500;
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

        auto _env = env;
        _env.appendOrSet("XMAKE_THEME", "plain");

        ProjectExplorer::TaskHub::clearTasks(static_cast<const char *>(ProjectExplorer::Constants::TASK_CATEGORY_BUILDSYSTEM));
        setupProcess(command, _env, capture_stdo);

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
    auto XMakeProcess::handleProcessFinished(int code, QProcess::ExitStatus status) -> void {
        m_cancel_timer.stop();

        m_stdo = m_process->readAllStandardOutput();

        if (status == QProcess::NormalExit) {
            m_future.setProgressValue(1);
            m_future.reportFinished();
        } else {
            m_future.reportCanceled();
            m_future.reportFinished();
        }

        const auto elapsed_time = Utils::formatElapsedTime(m_elapsed.elapsed());

        Core::MessageManager::writeSilently(elapsed_time);

        Q_EMIT finished(code, status);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::handleProcessError(QProcess::ProcessError error) -> void {
        auto message     = QString {};
        auto command_str = m_current_command.toUserOutput();

        switch (error) {
            case QProcess::ProcessError::FailedToStart:
                message = tr("The process failed to start. ") +
                          tr("Either the invoked program \"%1\" is missing, or you may have "
                             "insufficient permissions to invoke the program.")
                              .arg(m_current_command.executable().toUserOutput());
                break;
            case QProcess::ProcessError::Crashed:
                message = tr("The process was ended forcefully.");
                break;
            case QProcess::ProcessError::Timedout: message = tr("Process timed out."); break;
            case QProcess::ProcessError::WriteError:
                message = tr(
                    "An error occurred when attempting to write to the process. For example, the "
                    "process may not be running, or it may have closed its input channel.");
                break;
            case QProcess::ProcessError::ReadError:
                message = tr("An error occurred when attempting to read from the process. For "
                             "example, the process may not be running.");
                break;
            case QProcess::ProcessError::UnknownError:
                message = tr("An unknown error in the process occurred.");
                break;
        }

        ProjectExplorer::TaskHub::addTask(
            ProjectExplorer::BuildSystemTask { ProjectExplorer::Task::TaskType::Error,
                                               QString { "%1\n%2" }.arg(message, command_str) });

        handleProcessFinished(-1, QProcess::CrashExit);
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
        if (m_process) disconnect(m_process.get());

        m_process = std::make_unique<Utils::QtcProcess>();

        connect(m_process.get(), &Utils::QtcProcess::finished, this, [this] {
            handleProcessFinished(m_process->exitCode(), m_process->exitStatus());
        });

        connect(m_process.get(),
                &Utils::QtcProcess::errorOccurred,
                this,
                &XMakeProcess::handleProcessError);

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
        QTC_ASSERT(m_process, return );

        auto data = m_process->readAllStandardOutput();

        Core::MessageManager::writeSilently(QString::fromLocal8Bit(data));

        Q_EMIT readyReadStandardOutput(data);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProcess::processStandardError() -> void {
        QTC_ASSERT(m_process, return );

        Core::MessageManager::writeSilently(
            QString::fromLocal8Bit(m_process->readAllStandardError()));
    }
} // namespace XMakeProjectManager::Internal

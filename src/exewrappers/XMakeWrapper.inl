#pragma once

#include "XMakeWrapper.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline Command::Command(const Utils::FilePath &exe,
                            const Utils::FilePath &workDir,
                            const QStringList &args)
        : m_cmd { exe, args }, m_work_dir { workDir } {}

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline Command::Command(Command &&) = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Command::operator=(Command &&) -> Command & = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline Command::Command(const Command &) = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Command::operator=(const Command &) -> Command & = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Command::cmdLine() const -> const Utils::CommandLine & { return m_cmd; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Command::workDir() const -> const Utils::FilePath & { return m_work_dir; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Command::executable() const -> Utils::FilePath { return m_cmd.executable(); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Command::arguments() const -> QStringList { return m_cmd.splitArguments(); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto Command::toUserOutput() const -> QString { return m_cmd.toUserOutput(); };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    template<typename FileT>
    auto containsFiles(const QString &path, const FileT &file) -> bool {
        return QFile::exists(QString { "%1/%2" }.arg(path).arg(file));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    template<typename FileT, typename... T>
    auto containsFiles(const QString &path, const FileT &file, const T &...files) -> bool {
        return containsFiles(path, file) && containsFiles(path, files...);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto runXMake(const Command &command, QIODevice *output) -> bool {
        auto process = Utils::QtcProcess {};
        process.setWorkingDirectory(command.workDir());
        process.setCommand(command.cmdLine());
        process.start();

        if (!process.waitForFinished()) return false;

        if (output) output->write(process.readAllStandardOutput());

        return process.exitCode() == 0;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto isSetup(const Utils::FilePath &build_path) -> bool {
        using namespace Utils;

        return containsFiles(build_path.pathAppended(QLatin1String { Constants::XMAKE_INFO_DIR })
                                 .toString(),
                             QLatin1String { Constants::XMAKE_INTRO_CONF });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeWrapper::isValid() const noexcept -> bool { return m_is_valid; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeWrapper::autoDetected() const noexcept -> bool { return m_autodetected; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeWrapper::id() const noexcept -> const Utils::Id & { return m_id; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeWrapper::name() const noexcept -> QString { return m_name; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeWrapper::exe() const noexcept -> const Utils::FilePath & { return m_exe; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeWrapper::setName(QString new_name) noexcept -> void {
        m_name = std::move(new_name);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeWrapper::toolName() -> QString { return QStringLiteral("XMake"); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto toVariantMap(const XMakeWrapper &xmake) -> QVariantMap {
        auto data = QVariantMap {};
        data.insert(QLatin1String { Constants::ToolsSettings::NAME_KEY }, xmake.name());
        data.insert(QLatin1String { Constants::ToolsSettings::EXE_KEY }, xmake.exe().toVariant());
        data.insert(QLatin1String { Constants::ToolsSettings::AUTODETECTED_KEY },
                    xmake.autoDetected());
        data.insert(QLatin1String { Constants::ToolsSettings::UUID_KEY }, xmake.id().toSetting());
        data.insert(QLatin1String { Constants::ToolsSettings::TOOL_TYPE_KEY },
                    QLatin1String { Constants::ToolsSettings::TOOL_TYPE_XMAKE });

        return data;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto fromVariantMap(const QVariantMap &data) -> XMakeWrapper * {
        return new XMakeWrapper(
            data[QLatin1String { Constants::ToolsSettings::NAME_KEY }].toString(),
            Utils::FilePath::fromVariant(data[QLatin1String { Constants::ToolsSettings::EXE_KEY }]),
            Utils::Id::fromSetting(data[QLatin1String { Constants::ToolsSettings::UUID_KEY }]),
            data[QLatin1String { Constants::ToolsSettings::AUTODETECTED_KEY }].toBool());
    }
} // namespace XMakeProjectManager::Internal

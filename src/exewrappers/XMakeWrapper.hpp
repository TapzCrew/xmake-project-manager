#pragma once

#include <XMakeProjectConstant.hpp>
#include <XMakeProjectPlugin.hpp>

#include <utils/environment.h>
#include <utils/filepath.h>
#include <utils/fileutils.h>
#include <utils/id.h>
#include <utils/optional.h>
#include <utils/qtcprocess.h>

#include <QFile>
#include <QFileInfo>
#include <QTemporaryFile>

#include <tuple>

namespace XMakeProjectManager::Internal {
    template<typename FileT>
    bool containsFiles(const QString &path, const FileT &file);

    template<typename FileT, typename... T>
    bool containesFIles(const QString &path, const FileT &file, const T &...files);

    class Command {
      public:
        Command() = default;
        Command(const Utils::FilePath &exe,
                const Utils::FilePath &workDir,
                const QStringList &args);

        Command(Command &&)      = delete;
        Command(const Command &) = delete;

        Command &operator=(Command &&) = delete;
        Command &operator=(const Command &&) const = delete;

        const Utils::CommandLine &cmdLine() const;

        const Utils::FilePath &workDir() const;

        Utils::FilePath executable() const;

        QStringList arguments() const;

        QString toUserOutput() const;

      private:
        Utils::CommandLine m_cmd;
        Utils::FilePath m_work_dir;
    };

    bool runXMake(const Command &command, QIODevice *output = nullptr);
    bool isSetup(const Utils::FilePath &build_path);

    class XMakeWrapper {
      public:
        XMakeWrapper() = delete;
        XMakeWrapper(QString name, Utils::FilePath path, bool auto_detected = false);
        XMakeWrapper(QString name, Utils::FilePath path, Utils::Id id, bool auto_detected = false);
        ~XMakeWrapper();

        XMakeWrapper(XMakeWrapper &&);
        XMakeWrapper(const XMakeWrapper &);

        XMakeWrapper &operator=(const XMakeWrapper &);
        XMakeWrapper &operator=(XMakeWrapper &&);

        bool isValid() const noexcept;
        bool autoDetected() const noexcept;
        const Utils::Id &id() const noexcept;
        QString name() const noexcept;
        const Utils::FilePath &exe() const noexcept;

        void setName(QString new_name) noexcept;
        void setExe(Utils::FilePath new_exe);

        static Utils::optional<Utils::FilePath> findTool();

        Command configure(const Utils::FilePath &source_directory,
                          const Utils::FilePath &build_directory,
                          const QStringList &options = {}) const;

        Command introspect(const Utils::FilePath &source_directory);

        static QString toolName();

      private:
        bool m_is_valid;
        bool m_autodetected;
        Utils::Id m_id;
        Utils::FilePath m_exe;
        QString m_name;
    };

    QVariantMap toVariantMap(const XMakeWrapper &xmake);
    XMakeWrapper *fromVariantMap(const QVariantMap &data);
} // namespace XMakeProjectManager::Internal

#include "XMakeWrapper.inl"

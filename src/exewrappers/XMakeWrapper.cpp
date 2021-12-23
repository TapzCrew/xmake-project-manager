#include "XMakeWrapper.hpp"

#include <XMakeProjectConstant.hpp>

#include <coreplugin/icore.h>
#include <utils/qtcprocess.h>

#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUuid>

namespace XMakeProjectManager::Internal {
    static Q_LOGGING_CATEGORY(xmake_xmake_wrapper_log, "qtc.xmake.xmakewrapper", QtDebugMsg);

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    template<typename... T>
    auto options_cat(const T &...args) {
        auto result = QStringList {};

        (result.append(args), ...);

        return result;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeWrapper::XMakeWrapper(QString name, Utils::FilePath path, bool auto_detected)
        : m_is_valid { path.exists() }, m_autodetected { auto_detected },
          m_id { Utils::Id::fromString(QUuid::createUuid().toString()) }, m_exe { std::move(path) },
          m_name { std::move(name) } {}

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeWrapper::XMakeWrapper(QString name, Utils::FilePath path, Utils::Id id, bool auto_detected)
        : m_is_valid { path.exists() }, m_autodetected { auto_detected }, m_id { std::move(id) },
          m_exe { std::move(path) }, m_name { std::move(name) } {}

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeWrapper::~XMakeWrapper() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeWrapper::XMakeWrapper(XMakeWrapper &&) = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeWrapper::XMakeWrapper(const XMakeWrapper &) = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::operator=(XMakeWrapper &&) -> XMakeWrapper & = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::operator=(const XMakeWrapper &) -> XMakeWrapper & = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::setExe(Utils::FilePath new_exe) -> void { m_exe = std::move(new_exe); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::findTool() -> Utils::optional<Utils::FilePath> {
        using namespace Utils;

        auto env = Environment::systemEnvironment();

        const auto exe_path = env.searchInPath(QLatin1String { Constants::TOOL_NAME });
        if (exe_path.exists()) return exe_path;

        return Utils::nullopt;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::configure(const Utils::FilePath &source_directory,
                                 const Utils::FilePath &build_directory,
                                 const QStringList &options) const -> Command {
        return { m_exe,
                 Utils::FilePath::fromString(QDir::rootPath()), // source_directory,
                 options_cat("f",
                             "-P",
                             source_directory.toString(),
                             "-o",
                             build_directory.toString(),
                             options) };
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::introspect(const Utils::FilePath &source_directory) -> Command {
        auto path = decompressIntrospectLuaIfNot();

        return { m_exe,
                 Utils::FilePath::fromString(QDir::rootPath()), // source_directory,
                 options_cat("lua", "-P", source_directory.toString(), path) };
    }

    auto fileChecksum(const QString &fileName, QCryptographicHash::Algorithm hashAlgorithm)
        -> QByteArray {
        auto file = QFile { fileName };
        if (file.open(QFile::ReadOnly)) {
            auto hash = QCryptographicHash { hashAlgorithm };

            if (hash.addData(&file)) { return hash.result(); }
        }

        return {};
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto extractQrcFileTo(QString input, QString output) -> void {
        QString name = QFileInfo { input }.fileName();

        if (!QFileInfo::exists(output) || fileChecksum(input, QCryptographicHash::Sha256) !=
                                              fileChecksum(output, QCryptographicHash::Sha256)) {
            qCDebug(xmake_xmake_wrapper_log) << QString { "Extracting %1 to %2" }.arg(name, output);

            if (QFileInfo::exists(output)) QFile::remove(output);

            if (!QFile::copy(input, output)) {
                qCDebug(xmake_xmake_wrapper_log) << QString { "Failed to extract %1" }.arg(name);
            }
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::decompressIntrospectLuaIfNot() -> QString {
        auto dir = Core::ICore::userResourcePath("xmake-introspect-files");
        if (!QFileInfo::exists(dir.toString())) dir.createDir();

        auto introspect_path = dir.pathAppended("introspect.lua");
        extractQrcFileTo(":/xmakeproject/assets/introspect.lua", introspect_path.toString());

        return introspect_path.toString();
    }
} // namespace XMakeProjectManager::Internal

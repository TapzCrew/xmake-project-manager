#include "XMakeWrapper.hpp"

#include <XMakeProjectConstant.hpp>

#include <coreplugin/icore.h>
#include <utils/qtcprocess.h>

#include <QFile>
#include <QFileInfo>
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
                 build_directory,
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
                 source_directory,
                 options_cat("lua", "-P", source_directory.toString(), path) };
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeWrapper::decompressIntrospectLuaIfNot() -> QString {
        auto dir = Core::ICore::userResourcePath("xmake-introspect-files");
        if (!QFileInfo::exists(dir.toString())) dir.createDir();

        auto path = dir.pathAppended("introspect.lua");

        if (!QFileInfo::exists(path.toString())) {
            qCDebug(xmake_xmake_wrapper_log) << "Extracting introspect.lua to " << path;

            if (!QFile::copy(":/xmakeproject/assets/introspect.lua", path.toString())) {
                qCDebug(xmake_xmake_wrapper_log) << "Failed to extract introspect.lua";
                return "";
            }
        }

        return path.toString();
    }
} // namespace XMakeProjectManager::Internal

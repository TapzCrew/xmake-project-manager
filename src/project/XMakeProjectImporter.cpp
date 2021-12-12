#include "XMakeProjectImporter.hpp"

#include <QLoggingCategory>
#include <QString>

#include <projectexplorer/kit.h>

namespace {
    static Q_LOGGING_CATEGORY(mInputLog, "qtc.xmake.import", QtDebugMsg)
}

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProjectImporter::XMakeProjectImporter(const Utils::FilePath &path)
        : QtSupport::QtProjectImporter { path } {}

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectImporter::importCandidates() -> QStringList {
        // TODO
        return {};
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectImporter::examineDirectory(const Utils::FilePath &import_path,
                                                [[maybe_unused]] QString *warning_message) const
        -> QList<void *> {
        qCDebug(mInputLog()) << "examining build directory" << import_path.toUserOutput();

        // TODO
        auto data = QList<void *> {};

        return data;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectImporter::matchKit([[maybe_unused]] void *directory_data,
                                        [[maybe_unused]] const ProjectExplorer::Kit *kit) const
        -> bool {
        // TODO
        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectImporter::createKit([[maybe_unused]] void *directory_data) const
        -> ProjectExplorer::Kit * {
        // TODO
        return {};
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectImporter::buildInfoList([[maybe_unused]] void *directory_data) const
        -> const QList<ProjectExplorer::BuildInfo> {
        // TODO
        return {};
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectImporter::deleteDirectoryData([[maybe_unused]] void *directory_data) const
        -> void {
        // TODO
    }
} // namespace XMakeProjectManager::Internal

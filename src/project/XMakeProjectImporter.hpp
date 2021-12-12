#pragma once

#include <qtsupport/qtprojectimporter.h>

#include <projectexplorer/buildinfo.h>

#include <QStringList>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace ProjectExplorer {
    class Kit;
}

namespace XMakeProjectManager::Internal {
    class XMakeProjectImporter final: public QtSupport::QtProjectImporter {
      public:
        explicit XMakeProjectImporter(const Utils::FilePath &path);

        QStringList importCandidates() override;

      private:
        QList<void *> examineDirectory(const Utils::FilePath &import_path,
                                       QString *warning_message) const override;

        bool matchKit(void *directory_data, const ProjectExplorer::Kit *kit) const override;

        ProjectExplorer::Kit *createKit(void *directory_data) const override;

        const QList<ProjectExplorer::BuildInfo> buildInfoList(void *directory_data) const override;

        void deleteDirectoryData(void *directory_data) const override;
    };
} // namespace XMakeProjectManager::Internal

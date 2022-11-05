// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "utils/filepath.h"
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

        Utils::FilePaths importCandidates() override;

      private:
        QList<void *> examineDirectory(const Utils::FilePath &import_path,
                                       QString *warning_message) const override;

        bool matchKit(void *directory_data, const ProjectExplorer::Kit *kit) const override;

        ProjectExplorer::Kit *createKit(void *directory_data) const override;

        const QList<ProjectExplorer::BuildInfo> buildInfoList(void *directory_data) const override;

        void deleteDirectoryData(void *directory_data) const override;
    };
} // namespace XMakeProjectManager::Internal

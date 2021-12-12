#pragma once

#include <coreplugin/fileiconprovider.h>

#include <projectexplorer/projectnodes.h>

#include <utils/fileutils.h>

namespace XMakeProjectManager::Internal {
    class XMakeProjectNode final: public ProjectExplorer::ProjectNode {
      public:
        XMakeProjectNode(const Utils::FilePath &directory);
    };

    class XMakeTargetNode final: public ProjectExplorer::ProjectNode {
      public:
        XMakeTargetNode(const Utils::FilePath &directory, const QString &name);

        void build() override;

        QString tooltip() const override;
        QString buildKey() const override;

      private:
        QString m_name;
    };

    class XMakeFileNode final: public ProjectExplorer::ProjectNode {
      public:
        XMakeFileNode(const Utils::FilePath &file);

        bool showInSimpleTree() const override;

        Utils::optional<Utils::FilePath> visibleAfterAddFileAction() const override;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeProjectNodes.inl"

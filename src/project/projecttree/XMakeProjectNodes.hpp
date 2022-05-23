// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <coreplugin/fileiconprovider.h>

#include <projectexplorer/projectnodes.h>

#include <utils/fileutils.h>

namespace XMakeProjectManager::Internal {
    class XMakeProjectNode final: public ProjectExplorer::ProjectNode {
      public:
        XMakeProjectNode(const Utils::FilePath &directory);
    };

    class XMakeDependencyNode final: public ProjectExplorer::Node {
      public:
        XMakeDependencyNode(const Utils::FilePath &directory);

        QString displayName() const override;
        void setDisplayName(const QString &name);

        QIcon icon() const;
        void setIcon(const QIcon icon);

      private:
        QString m_name;

        mutable QIcon m_icon;
    };

    class XMakeTargetNode final: public ProjectExplorer::ProjectNode {
      public:
        XMakeTargetNode(const Utils::FilePath &directory,
                        const QString &name,
                        ProjectExplorer::ProductType type);

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

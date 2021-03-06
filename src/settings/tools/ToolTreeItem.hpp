// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <settings/tools/ToolsSettingsPage.hpp>

#include <utils/treemodel.h>

namespace XMakeProjectManager::Internal {
    class XMakeWrapper;
    class ToolTreeItem final: public Utils::TreeItem {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::ToolsSettingsPage)

      public:
        explicit ToolTreeItem(QString name);
        explicit ToolTreeItem(const XMakeWrapper &tool);
        ~ToolTreeItem();

        ToolTreeItem(ToolTreeItem &&) = delete;
        ToolTreeItem &operator=(ToolTreeItem &&) = delete;

        ToolTreeItem(const ToolTreeItem &);
        ToolTreeItem &operator=(const ToolTreeItem &);

        QVariant data(int column, int role) const override;

        const QString &name() const noexcept;

        const Utils::FilePath &executable() const noexcept;

        bool isAutoDetected() const noexcept;

        bool autorun() const noexcept;
        bool autoAcceptRequests() const noexcept;

        const Utils::Id &id() const noexcept;

        bool hasUnsavedChanges() const noexcept;
        void setSaved() noexcept;

        void update(QString name, Utils::FilePath exe, bool autorun, bool auto_accept_requests);

      private:
        void selfCheck();
        void updateTooltip();
        void clone(const ToolTreeItem &other);

        QString m_name;
        QString m_tooltip;

        Utils::FilePath m_executable;

        bool m_auto_detected;
        bool m_path_exists;
        bool m_path_is_file;
        bool m_path_is_executable;

        bool m_autorun;
        bool m_auto_accept_requests;

        Utils::Id m_id;

        bool m_unsaved_changes = false;
    };
} // namespace XMakeProjectManager::Internal

#include "ToolTreeItem.inl"

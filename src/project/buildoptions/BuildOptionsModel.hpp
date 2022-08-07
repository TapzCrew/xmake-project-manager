// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "../../xmakeinfoparser/XMakeBuildOptionsParser.hpp"

#include <utils/qtcassert.h>
#include <utils/treemodel.h>

#include <QAbstractTableModel>
#include <QFont>
#include <QItemEditorFactory>
#include <QStyledItemDelegate>
#include <QtCore/QObject>

namespace XMakeProjectManager::Internal {
    class CancellableOption {
      public:
        CancellableOption(const BuildOption &option, bool locked = false);

        bool locked() const noexcept;
        bool changed() const noexcept;

        void apply();
        void cancel();

        const QString &name() const noexcept;
        const QString &description() const noexcept;
        const QString &savedValue() const noexcept;
        const QStringList &values() const noexcept;

        QString xmakeArg() const noexcept;

        void setValue(const QString &value);
        const QString &value() const noexcept;

      private:
        bool m_changed;
        bool m_locked;

        std::unique_ptr<BuildOption> m_saved_value;
        std::unique_ptr<BuildOption> m_current_value;
    };
    using CancellableOptionsList = std::vector<std::unique_ptr<CancellableOption>>;

    class BuildOptionsModel final: public Utils::TreeModel<> {
        Q_OBJECT
      public:
        explicit BuildOptionsModel(QObject *parent = nullptr);

        void setConfiguration(const BuildOptionsList &options);
        bool setData(const QModelIndex &index, const QVariant &data, int role) override;

        QStringList changesAsXMakeArgs() const noexcept;

      Q_SIGNALS:
        void configurationChanged();

      private:
        bool changed() const noexcept;

        CancellableOptionsList m_options;
    };

    class BuildOptionTreeItem final: public Utils::TreeItem {
      public:
        explicit BuildOptionTreeItem(CancellableOption &option) noexcept;

        QVariant data(int column, int role) const noexcept;

        bool setData(int column, const QVariant &data, int role) noexcept;

        Qt::ItemFlags flags(int column) const noexcept;

        const QString &toolTip() const noexcept;

      private:
        CancellableOption *m_option;
    };

    class BuildOptionDelegate final: public QStyledItemDelegate {
        Q_OBJECT
      public:
        explicit BuildOptionDelegate(QObject *parent = nullptr);

        QWidget *createEditor(QWidget *parent,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const override;
        void setEditorData(QWidget *editor, const QModelIndex &index) const override;
        void setModelData(QWidget *editor,
                          QAbstractItemModel *model,
                          const QModelIndex &index) const override;

        QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    };
} // namespace XMakeProjectManager::Internal

#include "BuildOptionsModel.inl"

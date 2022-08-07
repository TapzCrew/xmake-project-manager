// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "BuildOptionsModel.hpp"
#include "qcombobox.h"
#include "qnamespace.h"

#include <algorithm>
#include <iterator>

#include <utils/treemodel.h>

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStyledItemDelegate>

namespace XMakeProjectManager::Internal {
    static constexpr auto LOCKED_OPTIONS =
        std::array<std::string_view, 4> { "mode", "qt", "plat", "arch" };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    BuildOptionsModel::BuildOptionsModel(QObject *parent) : Utils::TreeModel<> { parent } {
        setHeader({ tr("Key"), tr("Value") });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildOptionsModel::setConfiguration(const BuildOptionsList &options) -> void {
        clear();

        m_options.clear();

        m_options.reserve(std::size(options));
        std::transform(std::begin(options),
                       std::end(options),
                       std::back_inserter(m_options),
                       [](const auto &option) {
                           auto it =
                               std::find_if(std::begin(LOCKED_OPTIONS),
                                            std::end(LOCKED_OPTIONS),
                                            [name = option->name.toStdString()](
                                                const auto &opt_name) { return opt_name == name; });

                           return std::make_unique<CancellableOption>(*option,
                                                                      it !=
                                                                          std::end(LOCKED_OPTIONS));
                       });

        auto root = new Utils::TreeItem {};

        for (const auto &option : m_options) root->appendChild(new BuildOptionTreeItem { *option });

        setRootItem(root);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildOptionsModel::setData(const QModelIndex &index, const QVariant &data, int role)
        -> bool {
        const auto result = Utils::TreeModel<>::setData(index, data, role);

        if (changed()) Q_EMIT configurationChanged();

        return result;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildOptionsModel::changesAsXMakeArgs() const noexcept -> QStringList {
        auto args = QStringList {};

        for (const auto &option : m_options)
            args.emplace_back(option->xmakeArg()); // XMake doesn't support incremental config

        return args;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildOptionsModel::changed() const noexcept -> bool {
        for (const auto &option : m_options)
            if (option->changed()) return true;

        return false;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    BuildOptionDelegate::BuildOptionDelegate(QObject *parent) : QStyledItemDelegate { parent } {
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildOptionDelegate::createEditor(QWidget *parent,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const -> QWidget * {
        if (index.column() == 1) {
            const auto data      = index.data(Qt::EditRole);
            const auto read_only = index.data(Qt::UserRole).toBool();
            const auto values    = index.data(Qt::UserRole + 1).toStringList();

            if (values.size() == 2 && values.contains("false") && values.contains("true")) {
                auto edit = new QCheckBox { parent };
                edit->setFocusPolicy(Qt::StrongFocus);
                edit->setEnabled(read_only);

                return edit;
            } else if (!values.empty()) {
                const auto it = std::find(std::cbegin(values), std::cend(values), data);
                QTC_ASSERT(it != std::cend(values), return nullptr);

                const auto current_index = std::distance(std::begin(values), it);

                auto edit = new QComboBox { parent };
                edit->setAttribute(Qt::WA_MacSmallSize);
                edit->setFocusPolicy(Qt::StrongFocus);
                edit->setAutoFillBackground(true);
                edit->addItems(values);

                return edit;
            } else {
                auto edit = new QLineEdit { parent };
                edit->setFocusPolicy(Qt::StrongFocus);

                return edit;
            }
        }

        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    void BuildOptionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
        using namespace QtLiterals;

        if (index.column() == 1) {
            const auto data   = index.data(Qt::EditRole);
            const auto values = index.data(Qt::UserRole + 1).toStringList();

            const auto value = data.toString();

            if (values.size() == 6 && values.contains("false") && values.contains("true")) {
                const auto checked = value == "yes" || value == "y" || value == "true";

                auto edit = static_cast<QCheckBox *>(editor);
                edit->setChecked(checked);
                edit->setText(QString::fromLatin1((checked) ? "yes" : "no"));
                return;
            } else if (!values.empty()) {
                auto edit = static_cast<QComboBox *>(editor);
                edit->setCurrentText(value);
                return;
            } else {
                auto edit = static_cast<QLineEdit *>(editor);
                edit->setText(value);
                return;
            }
        }

        QStyledItemDelegate::setEditorData(editor, index);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    void BuildOptionDelegate::setModelData(QWidget *editor,
                                           QAbstractItemModel *model,
                                           const QModelIndex &index) const {
        if (index.column() == 1) {
            const auto values = index.data(Qt::UserRole + 1).toStringList();

            if (values.size() == 6 && values.contains("false") && values.contains("true")) {
                auto edit = static_cast<QCheckBox *>(editor);
                model->setData(index, edit->text(), Qt::EditRole);
                return;
            } else if (!values.empty()) {
                auto edit = static_cast<QComboBox *>(editor);
                model->setData(index, edit->currentText(), Qt::EditRole);
                return;
            } else {
                auto edit = static_cast<QLineEdit *>(editor);
                model->setData(index, edit->text(), Qt::EditRole);
                return;
            }
        }

        QStyledItemDelegate::setModelData(editor, model, index);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    QSize BuildOptionDelegate::sizeHint([[maybe_unused]] const QStyleOptionViewItem &option,
                                        [[maybe_unused]] const QModelIndex &index) const {
        static auto height = -1;
        if (height < 0) {
            const auto setMaxSize = [](const QWidget &w) {
                if (w.sizeHint().height() > height) height = w.sizeHint().height();
            };

            auto box = QComboBox {};
            box.setAttribute(Qt::WA_MacSmallSize);

            auto check = QCheckBox {};
            setMaxSize(box);
            setMaxSize(check);
        }
        return { 100, height };
    }
} // namespace XMakeProjectManager::Internal

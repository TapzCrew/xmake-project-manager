// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "BuildOptionsModel.hpp"
#include "qnamespace.h"
#include "qstyleditemdelegate.h"
#include "utils/treemodel.h"
#include <algorithm>
#include <iterator>

#include <QComboBox>
#include <QLineEdit>

namespace XMakeProjectManager::Internal {
    static constexpr auto LOCKED_OPTIONS = std::array<std::string_view, 2> { "mode", "qt" };

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
    BuildOptionDelegate::BuildOptionDelegate(QObject *parent) : QStyledItemDelegate { parent } {}

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildOptionDelegate::createEditor(QWidget *parent,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const -> QWidget * {
        const auto data      = index.data(Qt::EditRole);
        const auto read_only = index.data(Qt::UserRole).toBool();
        const auto values    = index.data(Qt::UserRole + 1).toStringList();
        auto widget          = makeWidget(parent, data, values);

        if (widget) {
            widget->setFocusPolicy(Qt::StrongFocus);
            widget->setDisabled(read_only);

            return widget;
        }

        return QStyledItemDelegate::createEditor(parent, option, index);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto BuildOptionDelegate::makeWidget(QWidget *parent,
                                         const QVariant &data,
                                         const QStringList &values) -> QWidget * {
        const auto value = data.toString();

        if (values.empty()) {
            auto widget = new QLineEdit { parent };
            widget->setText(value);
            return widget;
        }

        const auto it = std::find(std::cbegin(values), std::cend(values), value);
        QTC_ASSERT(it != std::cend(values), return nullptr);

        const auto current_index = std::distance(std::begin(values), it);

        auto widget = new QComboBox { parent };
        widget->addItems(values);
        widget->setCurrentIndex(current_index);

        return widget;
    }

} // namespace XMakeProjectManager::Internal

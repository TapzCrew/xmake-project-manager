// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeBuildSettingsWidget.hpp"
#include "../XMakeBuildConfiguration.hpp"
#include "../XMakeBuildSystem.hpp"
#include "project/buildoptions/BuildOptionsModel.hpp"
#include "projectexplorer/buildsystem.h"
#include "qnamespace.h"
#include "qregularexpression.h"
#include "ui_XMakeBuildSettingsWidget.h"
#include "utils/progressindicator.h"

#include <coreplugin/find/itemviewfind.h>

#include <projectexplorer/buildaspects.h>
#include <projectexplorer/projectconfiguration.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/target.h>

#include <utils/detailswidget.h>
#include <utils/headerviewstretcher.h>
#include <utils/layoutbuilder.h>

#include <QRegularExpression>

static constexpr auto REGEX = " -m [a-zA-Z]* ";

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildSettingsWidget::XMakeBuildSettingsWidget(XMakeBuildConfiguration *build_cfg)
        : ProjectExplorer::NamedWidget { tr("XMake Configuration"), nullptr },
          ui { new Ui::XMakeBuildSettingsWidget {} },
          m_progress_indicator { Utils::ProgressIndicatorSize::Large }, m_options_model { this } {
        ui->setupUi(this);
        ui->container->setState(Utils::DetailsWidget::NoSummary);
        ui->container->setWidget(ui->details);

        auto build_dir_builder = Utils::Layouting::Form {};
        build_cfg->buildDirectoryAspect()->addToLayout(build_dir_builder);
        build_dir_builder.attachTo(ui->build_dir_widget);

        auto parameters = build_cfg->parameters().toString();

        auto regex       = QRegularExpression { QString::fromLatin1(REGEX) };
        const auto match = regex.match(parameters);

        parameters.erase(parameters.begin() + match.capturedStart(),
                         parameters.begin() + match.capturedEnd());

        ui->parameters_line_edit->setText(parameters);
        ui->options_filter_line_edit->setFiltering(true);

        ui->options_tree_view->sortByColumn(0, Qt::AscendingOrder);

        auto find_wrapper =
            Core::ItemViewFind::createSearchableWrapper(ui->options_tree_view,
                                                        Core::ItemViewFind::LightColored);

        find_wrapper->setFrameStyle(QFrame::StyledPanel);

        m_progress_indicator.attachToWidget(find_wrapper);
        m_progress_indicator.raise();
        m_progress_indicator.hide();

        ui->details->layout()->addWidget(find_wrapper);

        m_show_progress_timer.setSingleShot(true);
        m_show_progress_timer.setInterval(50);

        connect(&m_show_progress_timer,
                &QTimer::timeout,
                &m_progress_indicator,
                &Utils::ProgressIndicator::show);
        connect(&m_options_model, &BuildOptionsModel::configurationChanged, this, [this] {
            ui->configure_button->setEnabled(true);
        });

        m_options_filter.setSourceModel(&m_options_model);
        m_options_filter.setSortRole(Qt::DisplayRole);
        m_options_filter.setFilterKeyColumn(-1);

        ui->options_tree_view->setModel(&m_options_filter);

        ui->options_tree_view->setItemDelegate(new BuildOptionDelegate { ui->options_tree_view });

        auto bs = static_cast<XMakeBuildSystem *>(build_cfg->buildSystem());
        connect(bs, &ProjectExplorer::BuildSystem::parsingFinished, this, [this, bs](bool success) {
            if (success) m_options_model.setConfiguration(bs->buildOptionsList());
            else
                m_options_model.clear();

            ui->options_tree_view->expandAll();
            ui->options_tree_view->resizeColumnToContents(0);
            ui->options_tree_view->setEnabled(true);
            m_show_progress_timer.stop();
            m_progress_indicator.hide();
        });

        connect(bs, &XMakeBuildSystem::parsingStarted, this, [this] {
            if (m_show_progress_timer.isActive()) {
                ui->options_tree_view->setEnabled(true);
                m_show_progress_timer.start();
            }
        });

        connect(&m_options_model, &BuildOptionsModel::dataChanged, this, [bs, this] {
            bs->setXMakeConfigArgs(m_options_model.changesAsXMakeArgs());
        });

        connect(&m_options_filter, &QAbstractItemModel::modelReset, this, [this] {
            ui->options_tree_view->expandAll();
            ui->options_tree_view->resizeColumnToContents(0);
        });

        connect(ui->options_filter_line_edit,
                &QLineEdit::textChanged,
                &m_options_filter,
                [this](const QString &txt) {
                    m_options_filter.setFilterRegularExpression(
                        QRegularExpression { QRegularExpression::escape(txt),
                                             QRegularExpression::CaseInsensitiveOption });
                });

        connect(ui->options_tree_view,
                &Utils::TreeView::activated,
                ui->options_tree_view,
                [tree = ui->options_tree_view](const QModelIndex &index) { tree->edit(index); });

        connect(ui->configure_button, &QPushButton::clicked, this, [bs, this] {
            ui->options_tree_view->setEnabled(false);
            ui->configure_button->setEnabled(false);

            m_show_progress_timer.start();

            bs->configure();
        });

        connect(ui->configure_button, &QPushButton::clicked, this, [bs, this] {
            ui->options_tree_view->setEnabled(false);
            ui->configure_button->setEnabled(false);

            m_show_progress_timer.start();

            bs->wipe();
        });

        connect(ui->parameters_line_edit, &QLineEdit::editingFinished, this, [this, build_cfg] {
            build_cfg->setParameters(ui->parameters_line_edit->text());
        });

        bs->triggerParsing();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildSettingsWidget::~XMakeBuildSettingsWidget() { delete ui; }

} // namespace XMakeProjectManager::Internal

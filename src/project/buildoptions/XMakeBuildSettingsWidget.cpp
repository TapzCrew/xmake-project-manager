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
#include "xmakeinfoparser/XMakeBuildOptionsParser.hpp"

#include <coreplugin/find/itemviewfind.h>

#include <projectexplorer/buildaspects.h>
#include <projectexplorer/projectconfiguration.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/target.h>

#include <utils/detailswidget.h>
#include <utils/headerviewstretcher.h>
#include <utils/layoutbuilder.h>

#include <QRegularExpression>

namespace XMakeProjectManager::Internal {
    static constexpr auto LOCKED_OPTIONS = std::array<std::string_view, 2> { "mode", "qt" };

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

        const auto _parameters = build_cfg->parameters().toString();
        const auto parameters  = _parameters.split(" ");

        for (const auto &parameter : parameters) {
            auto param_component = parameter.split("=");

            m_base_options.emplace_back(std::make_unique<BuildOption>(
                BuildOption { param_component[0].right(param_component[0].size() - 2),
                              "",
                              param_component[1].remove("\"") }));
        }

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
            m_options_model.clear();

            if (success) {
                const auto &_option_list = bs->buildOptionsList();

                auto option_list = BuildOptionsList {};
                option_list.reserve(std::size(m_base_options) + std::size(_option_list));
                for (const auto &option : m_base_options)
                    option_list.emplace_back(std::make_unique<BuildOption>(*option));
                for (const auto &option : _option_list)
                    option_list.emplace_back(std::make_unique<BuildOption>(*option));
                m_options_model.setConfiguration(option_list);
            }

            ui->options_tree_view->expandAll();
            ui->options_tree_view->resizeColumnToContents(0);
            ui->options_tree_view->setEnabled(true);
            m_show_progress_timer.stop();
            m_progress_indicator.hide();

            ui->wipe_button->setEnabled(true);
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
            ui->wipe_button->setEnabled(false);

            m_show_progress_timer.start();

            bs->configure();
        });

        connect(ui->wipe_button, &QPushButton::clicked, this, [bs, this] {
            ui->options_tree_view->setEnabled(false);
            ui->configure_button->setEnabled(false);
            ui->wipe_button->setEnabled(false);

            m_show_progress_timer.start();

            bs->wipe();
        });

        connect(ui->parameters_line_edit, &QLineEdit::editingFinished, this, [this, build_cfg] {
            build_cfg->addParameters(ui->parameters_line_edit->text());

            ui->parameters_line_edit->clear();
        });

        connect(build_cfg, &XMakeBuildConfiguration::parametersChanged, this, [this, build_cfg] {
            const auto _parameters = build_cfg->parameters().toString();
            const auto parameters  = _parameters.split(" ");

            for (const auto &parameter : parameters) {
                const auto param_component = parameter.split("=");
                const auto param_name  = param_component[0].right(param_component[0].size() - 2);
                const auto param_value = QString { param_component[1] }.remove("\"");

                if (std::any_of(std::begin(LOCKED_OPTIONS),
                                std::end(LOCKED_OPTIONS),
                                [&param_name](const auto &o) {
                                    return o == param_name.toStdString();
                                }))
                    continue;

                if (auto it =
                        std::find_if(m_base_options.begin(),
                                     m_base_options.end(),
                                     [param_name](const auto &o) { return o->name == param_name; });
                    it != m_base_options.end()) {
                    (*it)->value = param_value;
                } else
                    m_base_options.emplace_back(
                        std::make_unique<BuildOption>(BuildOption { param_name, "", param_value }));
            }
        });

        bs->triggerParsing();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildSettingsWidget::~XMakeBuildSettingsWidget() {
        delete ui;
    }

} // namespace XMakeProjectManager::Internal

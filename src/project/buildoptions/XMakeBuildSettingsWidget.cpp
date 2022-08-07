// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeBuildSettingsWidget.hpp"
#include "../XMakeBuildConfiguration.hpp"
#include "../XMakeBuildSystem.hpp"
#include "xmakeinfoparser/XMakeBuildOptionsParser.hpp"

#include <coreplugin/find/itemviewfind.h>

#include <projectexplorer/buildaspects.h>
#include <projectexplorer/buildsystem.h>
#include <projectexplorer/projectconfiguration.h>
#include <projectexplorer/projectexplorer.h>
#include <projectexplorer/target.h>

#include <project/buildoptions/BuildOptionsModel.hpp>

#include <utils/categorysortfiltermodel.h>
#include <utils/detailswidget.h>
#include <utils/fancylineedit.h>
#include <utils/headerviewstretcher.h>
#include <utils/itemviews.h>
#include <utils/layoutbuilder.h>
#include <utils/progressindicator.h>

#include <QtCore/QRegularExpression>

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

namespace XMakeProjectManager::Internal {
    static constexpr auto LOCKED_OPTIONS = std::array<std::string_view, 2> { "mode", "qt" };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildSettingsWidget::XMakeBuildSettingsWidget(XMakeBuildConfiguration *build_cfg)
        : ProjectExplorer::NamedWidget { tr("XMake Configuration"), nullptr } {
        auto vbox = new QVBoxLayout { this };
        vbox->setContentsMargins(0, 0, 0, 0);

        auto container = new Utils::DetailsWidget {};
        container->setState(Utils::DetailsWidget::NoSummary);

        vbox->addWidget(container);

        auto details = new QWidget { container };
        container->setWidget(details);

        auto build_dir_aspect = build_cfg->buildDirectoryAspect();
        build_dir_aspect->setAutoApplyOnEditingFinished(true);

        /*auto build_type_aspect = build_cfg->aspect<BuildTypeAspect>();
        connect(build_type_aspect, &Utils::BaseAspect::changed, this, [this, build_type_aspect] {
            m_options_model.setData();
        });*/

        /*m_kit_configuration = new QPushButton(tr("Kit Configuration"));
        m_kit_configuration->setToolTip(tr("Edit the current kit's XMake configuration."));
        m_kit_configuration->setFixedWidth(m_kit_configuration->sizeHint().width());
        QObject::connect(m_kit_configuration, &QPushButton::clicked, this, )*/

        m_options_filter = new Utils::CategorySortFilterModel { this };
        m_options_filter->setSourceModel(&m_options_model);
        m_options_filter->setSortRole(Qt::DisplayRole);
        m_options_filter->setFilterKeyColumn(-1);
        QObject::connect(m_options_filter, &QAbstractItemModel::layoutChanged, this, [this] {
            auto index = m_options_tree_view->currentIndex();
            if (index.isValid()) m_options_tree_view->scrollTo(index);
        });

        m_options_filter_line_edit = new Utils::FancyLineEdit {};
        m_options_filter_line_edit->setPlaceholderText(tr("Filter"));
        m_options_filter_line_edit->setFiltering(true);
        QObject::connect(m_options_filter_line_edit,
                         &QLineEdit::textChanged,
                         m_options_filter,
                         [this](const QString &txt) {
                             m_options_filter->setFilterRegularExpression(
                                 QRegularExpression { QRegularExpression::escape(txt),
                                                      QRegularExpression::CaseInsensitiveOption });
                         });

        m_options_tree_view = new Utils::TreeView {};
        QObject::connect(m_options_tree_view,
                         &Utils::TreeView::activated,
                         this,
                         [this](const auto &index) { m_options_tree_view->edit(index); });
        m_options_tree_view->viewport()->installEventFilter(this);
        m_options_tree_view->setModel(m_options_filter);
        m_options_tree_view->setMinimumHeight(300);
        m_options_tree_view->setUniformRowHeights(true);
        m_options_tree_view->sortByColumn(0, Qt::AscendingOrder);
        m_options_tree_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
        m_options_tree_view->setSelectionBehavior(QAbstractItemView::SelectItems);
        m_options_tree_view->setAlternatingRowColors(true);
        m_options_tree_view->setFrameShape(QFrame::NoFrame);
        m_options_tree_view->setItemDelegate(new BuildOptionDelegate { m_options_tree_view });
        m_options_tree_view->setRootIsDecorated(false);

        Utils::HeaderViewStretcher { m_options_tree_view->header(), 0 };

        auto find_wrapper =
            Core::ItemViewFind::createSearchableWrapper(m_options_tree_view,
                                                        Core::ItemViewFind::LightColored);
        find_wrapper->setFrameStyle(QFrame::StyledPanel);

        m_progress_indicator =
            new Utils::ProgressIndicator { Utils::ProgressIndicatorSize::Large, find_wrapper };
        m_progress_indicator->attachToWidget(find_wrapper);
        m_progress_indicator->raise();
        m_progress_indicator->hide();

        m_show_progress_timer.setSingleShot(true);
        m_show_progress_timer.setInterval(50);

        connect(&m_show_progress_timer,
                &QTimer::timeout,
                m_progress_indicator,
                &Utils::ProgressIndicator::show);
        connect(&m_options_model, &BuildOptionsModel::configurationChanged, this, [this] {
            m_configure_button->setEnabled(true);
        });

        m_configure_button = new QPushButton { tr("Run XMake") };
        m_configure_button->setEnabled(false);

        m_wipe_button = new QPushButton { tr("Wipe") };
        m_wipe_button->setEnabled(false);

        m_parameters_line_edit = new QLineEdit {};
        m_parameters_line_edit->setPlaceholderText("XMake options");

        {
            using namespace Utils::Layouting;

            Grid xmake_configuration { m_options_filter_line_edit,
                                       Break {},
                                       m_parameters_line_edit,
                                       Break {},
                                       find_wrapper };

            Column { Form { build_dir_aspect },
                     Column { Group { xmake_configuration,
                                      Row { m_options_tree_view },
                                      m_configure_button,
                                      m_wipe_button

                              } }
                         .setSpacing(0) }
                .attachTo(details, false);
        }

        auto bs = static_cast<XMakeBuildSystem *>(build_cfg->buildSystem());

        connect(bs, &XMakeBuildSystem::parsingStarted, this, [this] {
            if (m_show_progress_timer.isActive()) {
                m_options_tree_view->setEnabled(true);
                m_show_progress_timer.start();
            }
        });

        bs->triggerParsing();
        if (bs->isParsing()) m_show_progress_timer.start();
        else { m_options_model.setConfiguration(bs->buildOptionsList()); }

        connect(bs, &ProjectExplorer::BuildSystem::parsingFinished, this, [this, bs] {
            m_options_model.setConfiguration(bs->buildOptionsList());
            m_options_tree_view->setEnabled(true);
            m_show_progress_timer.stop();
            m_progress_indicator->hide();

            m_wipe_button->setEnabled(true);
        });

        QObject::connect(bs, &XMakeBuildSystem::errorOccurred, this, [this] {
            m_show_progress_timer.stop();
            m_progress_indicator->hide();
        });

        QObject::connect(&m_options_model, &BuildOptionsModel::dataChanged, this, [bs, this] {
            bs->setXMakeConfigArgs(m_options_model.changesAsXMakeArgs());
        });

        QObject::connect(m_options_filter, &QAbstractItemModel::modelReset, this, [this] {
            m_options_tree_view->expandAll();
            m_options_tree_view->resizeColumnToContents(0);
        });

        QObject::connect(m_options_tree_view,
                         &Utils::TreeView::activated,
                         m_options_tree_view,
                         [tree = m_options_tree_view](const QModelIndex &index) {
                             tree->edit(index);
                         });

        QObject::connect(m_configure_button, &QPushButton::clicked, this, [bs, this] {
            if (!bs->isParsing()) bs->configure();
            else
                m_configure_button->setEnabled(false);
        });

        QObject::connect(m_wipe_button, &QPushButton::clicked, this, [bs, this] {
            m_options_tree_view->setEnabled(false);
            m_configure_button->setEnabled(false);
            m_wipe_button->setEnabled(false);

            m_show_progress_timer.start();

            bs->wipe();
        });

        QObject::connect(m_parameters_line_edit,
                         &QLineEdit::editingFinished,
                         this,
                         [this, build_cfg] {
                             build_cfg->addParameters(m_parameters_line_edit->text());

                             m_parameters_line_edit->clear();
                         });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildSettingsWidget::~XMakeBuildSettingsWidget() = default;
} // namespace XMakeProjectManager::Internal

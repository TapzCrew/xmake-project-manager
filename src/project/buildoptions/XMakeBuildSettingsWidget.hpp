// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution
#pragma once

#include "BuildOptionsModel.hpp"

#include <projectexplorer/namedwidget.h>

#include <utils/categorysortfiltermodel.h>
#include <utils/progressindicator.h>

#include <QtCore/QTimer>

namespace XMakeProjectManager::Internal {
    namespace Ui {
        class XMakeBuildSettingsWidget;
    }
    class XMakeBuildConfiguration;
    class XMakeBuildSettingsWidget: public ProjectExplorer::NamedWidget {
        Q_OBJECT

      public:
        explicit XMakeBuildSettingsWidget(XMakeBuildConfiguration *build_cfg);
        ~XMakeBuildSettingsWidget();

      private:
        Ui::XMakeBuildSettingsWidget *ui;

        Utils::CategorySortFilterModel m_options_filter;
        Utils::ProgressIndicator m_progress_indicator;
        QTimer m_show_progress_timer;

        BuildOptionsModel m_options_model;
    };
} // namespace XMakeProjectManager::Internal

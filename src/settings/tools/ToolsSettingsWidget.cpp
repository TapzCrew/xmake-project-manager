#include "ToolsSettingsWidget.hpp"
#include "ui_ToolsSettingsWidget.h"

#include <XMakeProjectConstant.hpp>

#include <settings/tools/ToolTreeItem.hpp>
#include <settings/tools/ToolsItemSettings.hpp>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolsSettingsWidget::ToolsSettingsWidget() : Core::IOptionsPageWidget {} {
        m_ui = std::make_unique<Ui::ToolsSettingsWidget>();

        m_ui->setupUi(this);
        m_ui->xmake_details->setState(Utils::DetailsWidget::NoSummary);
        m_ui->xmake_details->setVisible(false);

        m_item_settings = new ToolItemSettings {};
        m_ui->xmake_details->setWidget(m_item_settings);

        m_ui->xmake_list->setModel(&m_model);
        m_ui->xmake_list->expandAll();
        m_ui->xmake_list->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        m_ui->xmake_list->header()->setSectionResizeMode(1, QHeaderView::Stretch);

        connect(m_ui->xmake_list->selectionModel(),
                &QItemSelectionModel::currentChanged,
                this,
                &ToolsSettingsWidget::currentXMakeToolChanged);
        connect(m_item_settings,
                &ToolItemSettings::applyChanges,
                &m_model,
                &ToolsModel::updateItem);

        connect(m_ui->add_button,
                &QPushButton::clicked,
                &m_model,
                QOverload<>::of(&ToolsModel::addXMakeTool));
        connect(m_ui->clone_button,
                &QPushButton::clicked,
                this,
                &ToolsSettingsWidget::cloneXMakeTool);
        connect(m_ui->remove_button,
                &QPushButton::clicked,
                this,
                &ToolsSettingsWidget::removeXMakeTool);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolsSettingsWidget::~ToolsSettingsWidget() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsSettingsWidget::apply() -> void { m_model.apply(); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsSettingsWidget::cloneXMakeTool() -> void {
        if (m_current_item) {
            auto new_item = m_model.cloneXMakeTool(m_current_item);

            m_ui->xmake_list->setCurrentIndex(new_item->index());
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsSettingsWidget::removeXMakeTool() -> void {
        if (m_current_item) m_model.removeXMakeTool(m_current_item);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsSettingsWidget::currentXMakeToolChanged(const QModelIndex &new_current) -> void {
        m_current_item = m_model.xmakeToolTreeItem(new_current);

        m_item_settings->load(m_current_item);

        m_ui->xmake_details->setVisible(m_current_item != nullptr);
        m_ui->clone_button->setEnabled(m_current_item != nullptr);
        m_ui->remove_button->setEnabled(m_current_item != nullptr &&
                                        !m_current_item->isAutoDetected());
    }
} // namespace XMakeProjectManager::Internal

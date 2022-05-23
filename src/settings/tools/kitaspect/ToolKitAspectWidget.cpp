// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "ToolKitAspectWidget.hpp"

#include <XMakeProjectConstant.hpp>

#include <exewrappers/XMakeTools.hpp>
#include <exewrappers/XMakeWrapper.hpp>

#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <QComboBox>

#include <utils/qtcassert.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolKitAspectWidget::ToolKitAspectWidget(ProjectExplorer::Kit *kit,
                                             const ProjectExplorer::KitAspect *ki,
                                             ToolType type)
        : KitAspectWidget { kit, ki }, m_tool_combo_box { createSubWidget<QComboBox>() },
          m_manager_button { createManageButton(Constants::SettingsPage::TOOLS_ID) }, m_type {
              type
          } {
        m_tool_combo_box->setSizePolicy(QSizePolicy::Ignored,
                                        m_tool_combo_box->sizePolicy().verticalPolicy());
        m_tool_combo_box->setEnabled(false);
        m_tool_combo_box->setToolTip(ki->description());

        loadTools();

        connect(&XMakeTools::instance(),
                &XMakeTools::toolAdded,
                this,
                &ToolKitAspectWidget::addTool);
        connect(&XMakeTools::instance(),
                &XMakeTools::toolRemoved,
                this,
                &ToolKitAspectWidget::removeTool);
        connect(m_tool_combo_box.get(),
                QOverload<int>::of(&QComboBox::currentIndexChanged),
                this,
                &ToolKitAspectWidget::setCurrentToolIndex);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::addTool(const XMakeWrapper &tool) -> void {
        if (isCompatible(tool)) m_tool_combo_box->addItem(tool.name(), tool.id().toSetting());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::removeTool(const XMakeWrapper &tool) -> void {
        if (!isCompatible(tool)) return;

        const auto index = indexOf(tool.id());
        QTC_ASSERT(index >= 0, return );

        if (index == m_tool_combo_box->currentIndex()) setToDefault();

        m_tool_combo_box->removeItem(index);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::setCurrentToolIndex(int index) -> void {
        const auto id = Utils::Id::fromSetting(m_tool_combo_box->itemData(index));

        if (m_type == ToolType::XMake) XMakeToolKitAspect::setXMakeTool(m_kit, id);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::indexOf(const Utils::Id &id) -> int {
        for (auto i = 0; i < m_tool_combo_box->count(); ++i)
            if (id == Utils::Id::fromSetting(m_tool_combo_box->itemData(i))) return i;

        return -1;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::isCompatible(const XMakeWrapper &tool) -> bool {
        return (m_type == ToolType::XMake);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::loadTools() -> void {
        for (const auto &tool : XMakeTools::tools()) addTool(*tool);

        refresh();

        m_tool_combo_box->setEnabled(m_tool_combo_box->count() > 0);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::setToDefault() -> void {
        const auto auto_detected = [this]() -> XMakeWrapper * {
            if (m_type == ToolType::XMake) return XMakeTools::xmakeWrapper();

            return nullptr;
        }();

        if (auto_detected) {
            const auto index = indexOf(auto_detected->id());

            m_tool_combo_box->setCurrentIndex(index);

            setCurrentToolIndex(index);
        } else {
            m_tool_combo_box->setCurrentIndex(0);

            setCurrentToolIndex(0);
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::makeReadOnly() -> void { m_tool_combo_box->setEnabled(false); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::addToLayout(Utils::LayoutBuilder &builder) -> void {
        addMutableAction(m_tool_combo_box.get());

        builder.addItem(m_tool_combo_box.get());
        builder.addItem(m_manager_button.get());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolKitAspectWidget::refresh() -> void {
        const auto id = [this]() -> Utils::Id {
            if (m_type == ToolType::XMake) return XMakeToolKitAspect::xmakeToolId(m_kit);

            return {};
        }();

        if (id.isValid()) m_tool_combo_box->setCurrentIndex(indexOf(id));
        else
            setToDefault();
    }
} // namespace XMakeProjectManager::Internal

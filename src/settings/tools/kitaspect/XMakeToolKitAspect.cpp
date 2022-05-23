// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeToolKitAspect.hpp"

#include <XMakeProjectConstant.hpp>

#include <settings/tools/kitaspect/ToolKitAspectWidget.hpp>

#include <utils/qtcassert.h>

#include <projectexplorer/kitmanager.h>
#include <projectexplorer/task.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeToolKitAspect::XMakeToolKitAspect() {
        setObjectName(QLatin1String("XMakeKitAspect"));
        setId(Constants::TOOL_ID);
        setDisplayName(tr("XMake Tool"));
        setDescription(tr("The XMake tool to use when building a project with XMake.<br>"
                          "This setting is ignored when using other build systems."));
        setPriority(9000);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeToolKitAspect::validate(const ProjectExplorer::Kit *kit) const
        -> ProjectExplorer::Tasks {
        auto tasks = ProjectExplorer::Tasks {};

        const auto tool = xmakeTool(kit);

        if (tool && !tool->isValid())
            tasks << ProjectExplorer::BuildSystemTask(ProjectExplorer::Task::Warning,
                                                      tr("Cannot validate this meson executable."));

        return tasks;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeToolKitAspect::setup(ProjectExplorer::Kit *kit) -> void {
        const auto tool = xmakeTool(kit);

        if (!tool) {
            const auto auto_detected = XMakeTools::xmakeWrapper();

            if (auto_detected) setXMakeTool(kit, auto_detected->id());
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeToolKitAspect::fix(ProjectExplorer::Kit *kit) -> void { setup(kit); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeToolKitAspect::toUserOutput(const ProjectExplorer::Kit *kit) const -> ItemList {
        const auto tool = xmakeTool(kit);

        if (tool) return { { tr("XMake"), tool->name() } };

        return { { tr("XMake"), tr("Unconfigured") } };
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeToolKitAspect::createConfigWidget(ProjectExplorer::Kit *kit) const
        -> ProjectExplorer::KitAspectWidget * {
        QTC_ASSERT(kit, return nullptr);

        return new ToolKitAspectWidget { kit, this, ToolKitAspectWidget::ToolType::XMake };
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeToolKitAspect::setXMakeTool(ProjectExplorer::Kit *kit, const Utils::Id &id) -> void {
        QTC_ASSERT(kit && id.isValid(), return );

        kit->setValue(Constants::TOOL_ID, id.toSetting());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeToolKitAspect::xmakeToolId(const ProjectExplorer::Kit *kit) -> Utils::Id {
        QTC_ASSERT(kit, return {});

        return Utils::Id::fromSetting(kit->value(Constants::TOOL_ID));
    }
} // namespace XMakeProjectManager::Internal

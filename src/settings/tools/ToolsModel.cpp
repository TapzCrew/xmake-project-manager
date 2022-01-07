#include "ToolsModel.hpp"

#include <settings/tools/ToolTreeItem.hpp>

#include <exewrappers/XMakeTools.hpp>
#include <exewrappers/XMakeWrapper.hpp>

#include <utils/qtcassert.h>
#include <utils/stringutils.h>

#include <projectexplorer/projectexplorerconstants.h>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolsModel::ToolsModel() {
        setHeader({ tr("Name"), tr("Location") });
        rootItem()->appendChild(
            new Utils::StaticTreeItem({ ProjectExplorer::Constants::msgAutoDetected() },
                                      { ProjectExplorer::Constants::msgAutoDetectedToolTip() }));
        rootItem()->appendChild(new Utils::StaticTreeItem(ProjectExplorer::Constants::msgManual()));

        for (const auto &tool : XMakeTools::tools()) { addXMakeTool(*tool); }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::xmakeToolTreeItem(const QModelIndex &index) const -> ToolTreeItem * {
        return itemForIndexAtLevel<2>(index);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::updateItem(const Utils::Id &item_id,
                                const QString &name,
                                const Utils::FilePath &exe,
                                bool autorun,
                                bool auto_accept_requests) -> void {
        auto item = findItemAtLevel<2>([&item_id](auto n) { return n->id() == item_id; });

        QTC_ASSERT(item, return );

        item->update(name, exe, autorun, auto_accept_requests);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::addXMakeTool() -> void {
        manualGroup()->appendChild(new ToolTreeItem { uniqueName(tr("New XMake tool")) });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::removeXMakeTool(ToolTreeItem *item) -> void {
        QTC_ASSERT(item, return );

        const auto &id = item->id();

        destroyItem(item);

        m_items_to_remove.enqueue(id);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::cloneXMakeTool(ToolTreeItem *item) -> ToolTreeItem * {
        QTC_ASSERT(item, return nullptr);

        auto new_item = new ToolTreeItem { *item };

        manualGroup()->appendChild(new_item);

        return new_item;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::apply() -> void {
        forItemsAtLevel<2>([this](auto item) {
            if (item->hasUnsavedChanges()) {
                XMakeTools::updateTool(item->id(),
                                       item->name(),
                                       item->executable(),
                                       item->autorun(),
                                       item->autoAcceptRequests());

                item->setSaved();

                Q_EMIT this->dataChanged(item->index(), item->index());
            }
        });

        while (!m_items_to_remove.empty()) XMakeTools::removeTool(m_items_to_remove.dequeue());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::addXMakeTool(const XMakeWrapper &tool) -> void {
        using TreeItemPtr = Utils::TreeItem *;
        auto group        = TreeItemPtr { nullptr };

        if (tool.autoDetected()) group = autoDetectedGroup();
        else
            group = manualGroup();

        group->appendChild(new ToolTreeItem(tool));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::uniqueName(const QString &base_name) -> QString {
        auto names = QStringList {};

        forItemsAtLevel<2>([&names](auto item) { names << item->name(); });

        return Utils::makeUniquelyNumbered(base_name, names);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::autoDetectedGroup() const -> Utils::TreeItem * {
        return rootItem()->childAt(0);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolsModel::manualGroup() const -> Utils::TreeItem * { return rootItem()->childAt(1); }
} // namespace XMakeProjectManager::Internal

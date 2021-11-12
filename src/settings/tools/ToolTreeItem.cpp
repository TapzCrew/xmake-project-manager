#include "ToolTreeItem.hpp"

#include <exewrappers/XMakeWrapper.hpp>

#include <utils/utilsicons.h>

#include <QFileInfo>
#include <QUuid>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolTreeItem::ToolTreeItem(QString name)
        : m_name { std::move(name) },
          m_auto_detected { false }, m_id { Utils::Id::fromString(QUuid::createUuid().toString()) },
          m_unsaved_changes { true } {
        selfCheck();
        updateTooltip();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolTreeItem::ToolTreeItem(const XMakeWrapper &tool)
        : m_name { tool.name() }, m_executable { tool.exe() },
          m_auto_detected { tool.autoDetected() }, m_id { tool.id() } {
        selfCheck();
        updateTooltip();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolTreeItem::~ToolTreeItem() = default;

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ToolTreeItem::ToolTreeItem(const ToolTreeItem &other) { clone(other); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolTreeItem::operator=(const ToolTreeItem &other) -> ToolTreeItem & {
        if (this != &other) clone(other);

        return *this;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolTreeItem::data(int column, int role) const -> QVariant {
        switch (role) {
            case Qt::DisplayRole: {
                if (column == 0) return m_name;
                else if (column == 1)
                    return m_executable.toUserOutput();
            } break;

            case Qt::FontRole: {
                auto font = QFont {};
                font.setBold(m_unsaved_changes);

                return font;
            }

            case Qt::ToolTipRole: {
                if (!m_path_exists)
                    return QCoreApplication::translate(
                        "XMakeProjectManager::Internal::ToolTreeItem",
                        "XMake executable path does not exist.");

                else if (!m_path_is_file)
                    return QCoreApplication::translate(
                        "XMakeProjectManager::Internal::ToolTreeItem",
                        "XMake executable path is not a file.");

                else if (!m_path_is_executable)
                    return QCoreApplication::translate(
                        "XMakeProjectManager::Internal::ToolTreeItem",
                        "XMake executable path is not executable.");

                return m_tooltip;
            }

            case Qt::DecorationRole: {
                if (column == 0 && (!m_path_exists || !m_path_is_file || !m_path_is_executable))
                    return Utils::Icons::CRITICAL.icon();

                return {};
            }

            default: return {};
        }

        return {};
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolTreeItem::update(QString name, Utils::FilePath exe) -> void {
        m_unsaved_changes = true;

        m_name = std::move(name);

        if (exe != m_executable) {
            m_executable = std::move(exe);

            selfCheck();
            updateTooltip();
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolTreeItem::selfCheck() -> void {
        m_path_exists        = m_executable.exists();
        m_path_is_file       = m_executable.toFileInfo().isFile();
        m_path_is_executable = m_executable.toFileInfo().isExecutable();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolTreeItem::updateTooltip() -> void { m_tooltip = tr("Version: TODO"); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto ToolTreeItem::clone(const ToolTreeItem &other) -> void {
        m_name            = tr("Clone of %1").arg(other.m_name);
        m_executable      = other.m_executable;
        m_auto_detected   = false;
        m_id              = Utils::Id::fromString(QUuid::createUuid().toString());
        m_unsaved_changes = true;

        selfCheck();
        updateTooltip();
    }
} // namespace XMakeProjectManager::Internal

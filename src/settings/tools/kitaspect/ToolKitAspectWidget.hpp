#pragma once

#include <projectexplorer/kitmanager.h>
#include <utils/layoutbuilder.h>

QT_BEGIN_NAMESPACE
class QComboBox;
class QWidget;
QT_END_NAMESPACE

namespace XMakeProjectManager::Internal {
    class XMakeWrapper;
    class ToolKitAspectWidget final: public ProjectExplorer::KitAspectWidget {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::ToolKitAspect)

      public:
        enum class ToolType { XMake };

        ToolKitAspectWidget(ProjectExplorer::Kit *kit,
                            const ProjectExplorer::KitAspect *ki,
                            ToolType type);
        ~ToolKitAspectWidget() override;

        ToolKitAspectWidget(ToolKitAspectWidget &&)      = delete;
        ToolKitAspectWidget(const ToolKitAspectWidget &) = delete;

        ToolKitAspectWidget &operator=(ToolKitAspectWidget &&) = delete;
        ToolKitAspectWidget &operator=(const ToolKitAspectWidget &) = delete;

      private:
        void addTool(const XMakeWrapper &tool);
        void removeTool(const XMakeWrapper &tool);
        void setCurrentToolIndex(int index);
        int indexOf(const Utils::Id &id);
        bool isCompatible(const XMakeWrapper &tool);
        void loadTools();
        void setToDefault();
        void makeReadOnly() override;
        void addToLayout(Utils::LayoutBuilder &builder) override;
        void refresh() override;

        std::unique_ptr<QComboBox> m_tool_combo_box;
        std::unique_ptr<QWidget> m_manager_button;

        ToolType m_type;
    };
} // namespace XMakeProjectManager::Internal

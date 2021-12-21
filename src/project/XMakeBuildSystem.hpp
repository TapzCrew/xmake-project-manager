#pragma once

#include <projectexplorer/buildsystem.h>

#include <cppeditor/cppprojectupdater.h>
#include <utils/filesystemwatcher.h>

#include <project/XMakeProjectParser.hpp>

#include <kithelper/KitData.hpp>

#include <xmakeinfoparser/XMakeBuildOptionsParser.hpp>

namespace XMakeProjectManager::Internal {
    class XMakeBuildConfiguration;
    class XMakeBuildSystem final: public ProjectExplorer::BuildSystem {
        Q_OBJECT
      public:
        explicit XMakeBuildSystem(XMakeBuildConfiguration *build_conf);

        void triggerParsing() override;

        QString name() const override;

        bool configure();
        bool wipe();

        XMakeBuildConfiguration *xmakeBuildConfiguration();

        const BuildOptionsList &buildOptionsList() const noexcept;

        const TargetsList &targets() const noexcept;
        const QStringList &targetList() const noexcept;

        void setXMakeConfigArgs(QStringList args);

        const XMakeProjectParser &parser() const noexcept;

      private:
        void init();

        bool parseProject();

        void updateKit(ProjectExplorer::Kit *kit);

        void parsingCompleted(bool success);

        ProjectExplorer::Kit *kit();

        QStringList configArgs(bool is_setup);

        ProjectExplorer::BuildSystem::ParseGuard m_parse_guard;

        XMakeProjectParser m_parser;

        CppEditor::CppProjectUpdater m_cpp_code_model_updater;

        QStringList m_pending_config_args;

        Utils::FileSystemWatcher m_intro_watcher;

        KitData m_kit_data;

        BuildOptionsList m_build_options;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeBuildSystem.inl"

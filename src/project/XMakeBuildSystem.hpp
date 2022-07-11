// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <projectexplorer/buildsystem.h>

#include <cppeditor/cppprojectupdater.h>
#include <utils/filesystemwatcher.h>

#include <project/XMakeProjectParser.hpp>

#include <xmakeinfoparser/XMakeBuildOptionsParser.hpp>

#include <qtsupport/qtcppkitinfo.h>

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

        ProjectExplorer::MakeInstallCommand
            makeInstallCommand(const Utils::FilePath &install_root) const override;

      private:
        void init();

        bool parseProject();

        void updateKit(ProjectExplorer::Kit *kit);

        void parsingCompleted(bool success);

        ProjectExplorer::Kit *kit();

        QStringList configArgs(bool is_setup);

        void updateQMLCodeModel();

        ProjectExplorer::BuildSystem::ParseGuard m_parse_guard;

        XMakeProjectParser m_parser;

        CppEditor::CppProjectUpdater m_cpp_code_model_updater;

        QStringList m_pending_config_args;

        Utils::FileSystemWatcher m_intro_watcher;

        QtSupport::CppKitInfo m_kit_info;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeBuildSystem.inl"

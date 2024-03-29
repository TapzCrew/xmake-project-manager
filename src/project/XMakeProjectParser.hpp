// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QFuture>
#include <QObject>
#include <QQueue>

#include <tuple>

#include <projectexplorer/buildsystem.h>
#include <projectexplorer/kit.h>
#include <projectexplorer/rawprojectpart.h>

#include <project/XMakeProcess.hpp>
#include <project/parsers/XMakeOutputParser.hpp>

#include <project/projecttree/XMakeProjectNodes.hpp>

#include <xmakeinfoparser/XMakeBuildOptionsParser.hpp>
#include <xmakeinfoparser/XMakeInfoParser.hpp>

#include <utils/environment.h>

namespace ProjectExplorer {
    class Project;
}

namespace QtSupport {
    class CppKitInfo;
}

namespace XMakeProjectManager::Internal {
    class XMakeProjectParser: public QObject {
        Q_OBJECT

      public:
        XMakeProjectParser(const Utils::Id &xmake,
                           Utils::Environment env,
                           ProjectExplorer::Project *project);

        void setXMakeTool(const Utils::Id &xmake) noexcept;

        bool configure(const Utils::FilePath &source_path,
                       const Utils::FilePath &build_path,
                       const QStringList &args,
                       bool wipe = false);

        bool wipe(const Utils::FilePath &source_path,
                  const Utils::FilePath &build_path,
                  const QStringList &args);

        bool parse(const Utils::FilePath &source_path, const Utils::FilePath &build_path);
        bool parse(const Utils::FilePath &source_path);

        std::unique_ptr<XMakeProjectNode> takeProjectNode() noexcept;

        const TargetsList &targets() const noexcept;
        const QStringList &targetsNames() const noexcept;

        const QStringList &qmlImportPaths() const noexcept;

        const BuildOptionsList &options() const noexcept;

        void setEnvironment(const Utils::Environment &environment);

        QList<ProjectExplorer::BuildTargetInfo> appTargets() const;
        ProjectExplorer::RawProjectParts
            buildProjectParts(const QtSupport::CppKitInfo &kit_info) const;

        const Utils::FilePath &srcDir() const noexcept;
        const Utils::FilePath &buildDir() const noexcept;
      Q_SIGNALS:
        void parsingCompleted(bool success);

      private:
        struct ParserData {
            XMakeInfoParser::Result data;
            std::unique_ptr<XMakeProjectNode> root_node;
        };

        bool startParser(const QByteArray &data);
        void processFinished(int code, QString std_out);

        static ParserData *extractParserResults(const Utils::FilePath &source_dir,
                                                XMakeInfoParser::Result &&parser_result);

        void update(const QFuture<ParserData *> &data);

        ProjectExplorer::RawProjectPart buildProjectPart(const Target &target,
                                                         const Target::SourceGroup &sources,
                                                         const QtSupport::CppKitInfo &kit_info,
                                                         int id) const;

        std::unique_ptr<XMakeProcess> m_process;
        bool m_configuring;

        QFuture<ParserData *> m_parser_future_result;

        XMakeOutputParser m_output_parser;

        Utils::Environment m_env;
        Utils::Id m_xmake;

        Utils::FilePath m_build_dir;
        Utils::FilePath m_src_dir;

        XMakeInfoParser::Result m_parser_result;
        QStringList m_targets_names;

        std::unique_ptr<XMakeProjectNode> m_root_node;

        QString m_project_name;

        QQueue<std::tuple<Command, Utils::FilePath, bool>> m_pending_commands;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeProjectParser.inl"

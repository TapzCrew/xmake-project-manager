#include "XMakeProjectParser.hpp"

#include <exewrappers/XMakeTools.hpp>
#include <project/projecttree/ProjectTree.hpp>

#include <coreplugin/messagemanager.h>

#include <projectexplorer/projectexplorer.h>

#include <utils/fileinprojectfinder.h>
#include <utils/runextensions.h>

#include <QStringList>

namespace XMakeProjectManager::Internal {
    static Q_LOGGING_CATEGORY(xmake_project_parser_log, "qtc.xmake.projectparser", QtDebugMsg);

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeProjectParser::XMakeProjectParser(const Utils::Id &xmake,
                                           Utils::Environment env,
                                           ProjectExplorer::Project *project)
        : m_xmake { xmake }, m_env { std::move(env) }, m_project_name { project->displayName() } {
        connect(&m_process, &XMakeProcess::finished, this, &XMakeProjectParser::processFinished);
        connect(&m_process,
                &XMakeProcess::readyReadStandardOutput,
                &m_output_parser,
                &XMakeOutputParser::readStdo);

        auto file_finder = new Utils::FileInProjectFinder {};
        file_finder->setProjectDirectory(project->projectDirectory());
        file_finder->setProjectFiles(project->files(ProjectExplorer::Project::AllFiles));

        m_output_parser.setFileFinder(file_finder);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::configure(const Utils::FilePath &source_path,
                                       const Utils::FilePath &build_path,
                                       const QStringList &args) -> bool {
        m_src_dir   = source_path;
        m_build_dir = build_path;

        m_output_parser.setSourceDirectory(source_path);

        auto cmd = XMakeTools::xmakeWrapper(m_xmake)->configure(source_path, build_path, args);

        return m_process.run(cmd, m_env, m_project_name, true);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::wipe(const Utils::FilePath &source_path,
                                  const Utils::FilePath &build_path,
                                  const QStringList &args) -> bool {
        return QFile::remove((build_path / ".xmake").toString());
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::parse(const Utils::FilePath &source_path,
                                   const Utils::FilePath &build_path) -> bool {
        m_src_dir   = source_path;
        m_build_dir = build_path;

        m_output_parser.setSourceDirectory(source_path);

        return parse(source_path);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::parse(const Utils::FilePath &source_path) -> bool {
        m_src_dir = source_path;

        m_output_parser.setSourceDirectory(source_path);

        auto cmd = XMakeTools::xmakeWrapper(m_xmake)->introspect(source_path);
        qCDebug(xmake_project_parser_log) << "Starting parser " << cmd.toUserOutput();

        return m_process.run(cmd,
                             m_env,
                             m_project_name,
                             true);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::startParser() -> bool {
        m_parser_future_result = Utils::runAsync(
            ProjectExplorer::ProjectExplorerPlugin::sharedThreadPool(),
            [process = &m_process, build_dir = m_build_dir.toString(), src_dir = m_src_dir] {
                return extractParserResults(src_dir, XMakeInfoParser::parse(process->stdOut()));
            });

        Utils::onFinished(m_parser_future_result, this, &XMakeProjectParser::update);

        return true;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::processFinished(int code, QProcess::ExitStatus status) -> void {
        qCDebug(xmake_project_parser_log) << "Process " << m_process.currentCommand().toUserOutput() << "finished with code: " << code << " status: " << status << " output: " << m_process.stdOut();

        if (code != 0 || status != QProcess::NormalExit) {
            const auto &data = m_process.stdOut();

            Core::MessageManager::writeSilently(QString::fromLocal8Bit(data));

            m_output_parser.readStdo(data);

            Q_EMIT parsingCompleted(false);

            return;
        }

        startParser();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::extractParserResults(const Utils::FilePath &src_dir,
                                                  XMakeInfoParser::Result &&parser_result)
        -> XMakeProjectParser::ParserData * {
        qCDebug(xmake_project_parser_log) << "Extract parser results";
        auto root_node = ProjectTree::buildTree(src_dir,
                                                parser_result.targets,
                                                parser_result.build_system_files);

        return new ParserData { std::move(parser_result), std::move(root_node) };
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::update(const QFuture<ParserData *> &data) -> void {
        auto parser_data = data.result();

        m_parser_result = std::move(parser_data->data);
        m_root_node     = std::move(parser_data->root_node);

        m_targets_names.clear();

        for (const auto &target : m_parser_result.targets)
            m_targets_names.emplace_back(target.name);

        m_targets_names.sort();

        delete parser_data;

        Q_EMIT parsingCompleted(true);
    }
} // namespace XMakeProjectManager::Internal

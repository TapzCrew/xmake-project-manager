#include "XMakeProjectParser.hpp"

#include <algorithm>

#include <exewrappers/XMakeTools.hpp>
#include <project/projecttree/ProjectTree.hpp>

#include <coreplugin/messagemanager.h>

#include <projectexplorer/projectexplorer.h>

#include <utils/fileinprojectfinder.h>
#include <utils/runextensions.h>

#include <QJsonDocument>
#include <QStringList>

namespace XMakeProjectManager::Internal {
    static Q_LOGGING_CATEGORY(xmake_project_parser_log, "qtc.xmake.projectparser", QtDebugMsg);

    struct CompilerArgs {
        QStringList arguments;
        ProjectExplorer::HeaderPaths include_paths;
        ProjectExplorer::Macros macros;
    };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto extractValueIfMatches(const QString &arg, const QStringList &candidates)
        -> Utils::optional<QString> {
        for (const auto &flag : candidates)
            if (arg.startsWith(flag)) return arg.mid(flag.length());

        return Utils::nullopt;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto extractInclude(const QString &arg) -> Utils::optional<ProjectExplorer::HeaderPath> {
        auto path = extractValueIfMatches(arg, { "-I", "/I", "-imsvc ", "/imsvc " });
        if (path) return ProjectExplorer::HeaderPath::makeUser(*path);

        path = extractValueIfMatches(arg, { "-isystem ", "/external:I", "-external:I" });
        if (path) return ProjectExplorer::HeaderPath::makeSystem(*path);

        return Utils::nullopt;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto extractMacro(const QString &arg) -> Utils::optional<ProjectExplorer::Macro> {
        auto define = extractValueIfMatches(arg, { "-D", "/D" });
        if (define) return ProjectExplorer::Macro::fromKeyValue(define->toLatin1());

        auto undef = extractValueIfMatches(arg, { "-U", "/U" });

        if (undef)
            return ProjectExplorer::Macro(undef->toLatin1(), ProjectExplorer::MacroType::Undefine);

        return Utils::nullopt;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto splitArgs(const QStringList &args, const Utils::FilePath &src_dir) -> CompilerArgs {
        auto splited = CompilerArgs {};

        for (const QString &arg : args) {
            auto inc = extractInclude(arg);
            if (inc) {
                auto path = Utils::FilePath::fromString(inc->path);
                if (!path.isAbsolutePath()) path = src_dir.resolvePath(path);

                splited.include_paths << ProjectExplorer::HeaderPath { path.path(), inc->type };
            } else {
                auto macro = extractMacro(arg);
                if (macro) splited.macros << *macro;
                else
                    splited.arguments << arg;
            }
        }

        return splited;
    }

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
                                       const QStringList &args,
                                       bool wipe) -> bool {
        m_src_dir   = source_path;
        m_build_dir = build_path;

        m_env.appendOrSet("XMAKE_CONFIGDIR", m_build_dir.path());

        m_output_parser.setSourceDirectory(source_path);

        auto cmd = XMakeTools::xmakeWrapper(m_xmake)->configure(m_src_dir, m_build_dir, args);

        m_pending_commands.enqueue(
            std::make_tuple(XMakeTools::xmakeWrapper(m_xmake)->introspect(source_path), true));

        return m_process.run(cmd, m_env, m_project_name, true);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::wipe(const Utils::FilePath &source_path,
                                  const Utils::FilePath &build_path,
                                  const QStringList &args) -> bool {
        return configure(source_path, build_path, args, true);
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

        m_env.appendOrSet("XMAKE_CONFIGDIR", m_build_dir.path());

        auto cmd = XMakeTools::xmakeWrapper(m_xmake)->introspect(source_path);
        qCDebug(xmake_project_parser_log) << "Starting parser " << cmd.toUserOutput();

        return m_process.run(cmd, m_env, m_project_name, true);
    }

    auto XMakeProjectParser::appTargets() const -> QList<ProjectExplorer::BuildTargetInfo> {
        auto apps = QList<ProjectExplorer::BuildTargetInfo> {};

        for (const auto &target : m_parser_result.targets) {
            if (target.kind == Target::Kind::BINARY) {
                auto &target_info = apps.emplace_back();

                auto target_file = m_src_dir.resolvePath(target.target_file).path();

                target_info.displayName = target.name;
                target_info.buildKey = Target::fullName(m_src_dir, target_file, target.defined_in);
                target_info.displayNameUniquifier = target_info.buildKey;
                target_info.targetFilePath        = Utils::FilePath::fromString(target_file);
                target_info.workingDirectory =
                    Utils::FilePath::fromString(target_file).absolutePath();
                target_info.projectFilePath = Utils::FilePath::fromString(target.defined_in);
                target_info.usesTerminal    = true;
            }
        }

        return apps;
    }

    auto XMakeProjectParser::buildProjectParts(const ProjectExplorer::ToolChain *cxx_toolchain,
                                               const ProjectExplorer::ToolChain *c_toolchain) const
        -> ProjectExplorer::RawProjectParts {
        auto parts = ProjectExplorer::RawProjectParts {};

        for (const auto &target : m_parser_result.targets) {
            std::transform(std::cbegin(target.sources),
                           std::cend(target.sources),
                           std::back_inserter(parts),
                           [&target, &cxx_toolchain, &c_toolchain, this](const auto &source_group) {
                               return buildProjectPart(target,
                                                       source_group,
                                                       cxx_toolchain,
                                                       c_toolchain);
                           });
        }

        return parts;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::startParser(const QByteArray &data) -> bool {
        m_parser_future_result =
            Utils::runAsync(ProjectExplorer::ProjectExplorerPlugin::sharedThreadPool(),
                            [data, build_dir = m_build_dir.toString(), src_dir = m_src_dir] {
                                return extractParserResults(src_dir, XMakeInfoParser::parse(data));
                            });

        Utils::onFinished(m_parser_future_result, this, &XMakeProjectParser::update);

        return true;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeProjectParser::processFinished(int code, QProcess::ExitStatus status) -> void {
        auto json_output = [this] {
            auto json = QJsonDocument::fromJson(m_process.stdOut());

            auto str = QString::fromLocal8Bit(json.toJson());

            if (str.size()) return str;

            return QString::fromLocal8Bit(m_process.stdOut());
        }();

        qCDebug(xmake_project_parser_log) << "Process " << m_process.currentCommand().toUserOutput()
                                          << "finished with code: " << code << " status: " << status
                                          << " output: " << json_output;

        const auto error_or_warning_regex =
            QRegularExpression { R"|(((error:)|(warning:))(.+)\n)|" };
        const auto error_regex   = QRegularExpression { R"|(error:(.+)\n)|" };
        const auto warning_regex = QRegularExpression { R"|(warning:(.+)\n)|" };

        auto has_error_or_warnings = error_or_warning_regex.match(json_output);
        auto error_or_warnings     = QString {};

        auto has_errors   = error_regex.match(json_output).hasMatch();
        auto has_warnings = warning_regex.match(json_output).hasMatch();
        while (has_error_or_warnings.hasMatch()) {
            error_or_warnings += has_error_or_warnings.captured(0);
            json_output.erase(std::begin(json_output) + has_error_or_warnings.capturedStart(0),
                              std::begin(json_output) + has_error_or_warnings.capturedEnd(0));

            has_error_or_warnings = error_or_warning_regex.match(json_output);
        }

        if (code != 0 || status != QProcess::NormalExit || has_errors) {
            const auto &data = m_process.stdOut();

            Core::MessageManager::writeSilently(error_or_warnings);

            m_output_parser.readStdo(error_or_warnings.toLocal8Bit());

            Q_EMIT parsingCompleted(false);

            return;
        } else if (has_warnings) {
            const auto &data = m_process.stdOut();

            Core::MessageManager::writeSilently(error_or_warnings);

            m_output_parser.readStdo(error_or_warnings.toLocal8Bit());
        }

        if (m_pending_commands.isEmpty()) startParser(json_output.toLocal8Bit());
        else {
            auto args = m_pending_commands.dequeue();
            m_process.run(std::get<0>(args), m_env, m_project_name, std::get<1>(args));
        }
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

        std::transform(std::cbegin(m_parser_result.targets),
                       std::cend(m_parser_result.targets),
                       std::back_inserter(m_targets_names),
                       [](const auto &target) { return target.name; });

        m_targets_names.sort();

        delete parser_data;

        Q_EMIT parsingCompleted(true);
    }

    auto XMakeProjectParser::buildProjectPart(const Target &target,
                                              const Target::SourceGroup &sources,
                                              const ProjectExplorer::ToolChain *cxx_toolchain,
                                              const ProjectExplorer::ToolChain *c_toolchain) const
        -> ProjectExplorer::RawProjectPart {
        const auto flags = splitArgs(sources.arguments, m_src_dir);
        qCDebug(xmake_project_parser_log)
            << "---------------- TARGET " << target.name << " -------------------";
        for (const auto &path : flags.include_paths)
            qCDebug(xmake_project_parser_log)
                << "INCLUDEPATH: " << path.path << ":" << static_cast<int>(path.type);
        for (const auto &macro : flags.macros)
            qCDebug(xmake_project_parser_log) << "MACRO: " << macro.key << ":" << macro.value;
        qCDebug(xmake_project_parser_log) << "ARGUMENTS: " << flags.arguments;
        qCDebug(xmake_project_parser_log) << "SOURCES: " << sources.sources;
        qCDebug(xmake_project_parser_log) << "SOURCES LANGUAGE: " << sources.language;

        auto target_file = m_src_dir.resolvePath(target.target_file).path();

        auto part = ProjectExplorer::RawProjectPart {};

        auto absolute_sources = QStringList {};
        absolute_sources.reserve(std::size(sources.sources));
        std::transform(std::cbegin(sources.sources),
                       std::cend(sources.sources),
                       std::back_inserter(absolute_sources),
                       [&src_dir = m_src_dir](const auto &file) {
                           return src_dir.resolvePath(file).path();
                       });

        part.setDisplayName(target.name);
        part.setBuildSystemTarget(Target::fullName(m_src_dir, target_file, target.defined_in));
        part.setFiles(absolute_sources + target.headers);
        part.setProjectFileLocation(target.defined_in);

        part.setHeaderPaths(flags.include_paths);
        part.setMacros(flags.macros);
        // part.setIncludedFiles(target.headers);

        auto base_dir = Utils::FilePath::fromString(target.defined_in).absolutePath().toString();
        if (sources.language == "cxx" || sources.language == "cxxmodule")
            part.setFlagsForCxx({ cxx_toolchain, flags.arguments, base_dir });
        else if (sources.language == "cc")
            part.setFlagsForC({ c_toolchain, flags.arguments, base_dir });

        part.setBuildTargetType((target.kind == Target::Kind::BINARY)
                                    ? ProjectExplorer::BuildTargetType::Executable
                                    : ProjectExplorer::BuildTargetType::Library);

        return part;
    }
} // namespace XMakeProjectManager::Internal

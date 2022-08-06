// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include "XMakeBuildStep.hpp"

#include <XMakeProjectConstant.hpp>
#include <project/XMakeBuildConfiguration.hpp>
#include <project/XMakeBuildSystem.hpp>
#include <project/XMakeProject.hpp>
#include <project/parsers/XMakeBuildParser.hpp>

#include <coreplugin/find/itemviewfind.h>

#include <settings/tools/kitaspect/XMakeToolKitAspect.hpp>

#include <qtsupport/qtcppkitinfo.h>

#include <projectexplorer/buildsteplist.h>
#include <projectexplorer/processparameters.h>
#include <projectexplorer/projectexplorerconstants.h>
#include <projectexplorer/target.h>

#include <utils/commandline.h>

#include <QFormLayout>
#include <QListWidget>
#include <QRadioButton>

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildStep::XMakeBuildStep(ProjectExplorer::BuildStepList *bsl, Utils::Id id)
        : ProjectExplorer::AbstractProcessStep { bsl, id } {
        if (m_target_name.isEmpty()) setBuildTarget(defaultBuildTarget());

        setLowPriority();

        setCommandLineProvider([this] { return command(); });
        setEnvironmentModifier([this](Utils::Environment &env) {
            env.setupEnglishOutput();
            env.appendOrSet("XMAKE_CONFIGDIR", buildDirectory().path());
            env.appendOrSet("XMAKE_THEME", "plain");
        });
        setWorkingDirectoryProvider([this] { return project()->projectDirectory(); });

        connect(target(), &ProjectExplorer::Target::parsingFinished, this, &XMakeBuildStep::update);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::createConfigWidget() -> QWidget * {
        auto widget = new QWidget {};
        setDisplayName(tr("Build"));

        auto build_targets_list = new QListWidget { widget };
        build_targets_list->setMinimumHeight(200);
        build_targets_list->setFrameShape(QFrame::StyledPanel);
        build_targets_list->setFrameShadow(QFrame::Raised);

        auto tool_arguments = new QLineEdit { widget };

        auto wrapper =
            Core::ItemViewFind::createSearchableWrapper(build_targets_list,
                                                        Core::ItemViewFind::LightColored);

        auto form_layout = new QFormLayout { widget };
        form_layout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        form_layout->setContentsMargins(0, 0, 0, 0);
        form_layout->addRow(tr("Tool arguments:"), tool_arguments);
        form_layout->addRow(tr("Targets:"), wrapper);

        auto update_details = [this] {
            auto param = ProjectExplorer::ProcessParameters {};
            setupProcessParameters(&param);
            setSummaryText(param.summary(displayName()));
        };

        auto update_target_list = [this, build_targets_list, update_details] {
            build_targets_list->clear();

            for (const auto &target : projectTargets()) {
                auto item   = new QListWidgetItem { build_targets_list };
                auto button = new QRadioButton { target };

                connect(button,
                        &QRadioButton::toggled,
                        this,
                        [this, target, update_details](auto toggled) {
                            if (!toggled) return;

                            setBuildTarget(target);
                            update_details();
                        });

                button->setChecked(targetName() == target);

                build_targets_list->setItemWidget(item, button);

                item->setData(Qt::UserRole, target);
            }
        };

        update_details();
        update_target_list();

        connect(this, &XMakeBuildStep::commandChanged, this, update_details);

        connect(this, &XMakeBuildStep::targetListChanged, widget, update_target_list);

        connect(tool_arguments,
                &QLineEdit::textEdited,
                this,
                [this, update_details](const auto &text) {
                    setCommandArgs(text);
                    update_details();
                });

        connect(build_targets_list,
                &QListWidget::itemChanged,
                this,
                [this, update_details](auto *item) {
                    if (item->checkState() != Qt::Checked) return;

                    setBuildTarget(item->data(Qt::UserRole).toString());
                    update_details();
                });

        return widget;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::command() -> Utils::CommandLine {
        auto cmd = [this] {
            auto tool = XMakeToolKitAspect::xmakeTool(kit());
            if (tool) return Utils::CommandLine { tool->exe() };

            return Utils::CommandLine {};
        }();

        qDebug() << m_target_name;

        if (!m_target_name.isEmpty() &&
            m_target_name == QString::fromLatin1(Constants::Targets::CLEAN)) {
            cmd.addArg("c");
        } else {
            cmd.addArg("b");

            if (!m_command_args.isEmpty())
                cmd.addArgs(m_command_args, Utils::CommandLine::RawType::Raw);
        }

        cmd.addArg("-P");

        cmd.addArg(QString { "%1" }.arg(project()->projectDirectory().path()));

        if (!m_target_name.isEmpty() &&
            m_target_name != QString::fromLatin1(Constants::Targets::ALL) &&
            m_target_name != QString::fromLatin1(Constants::Targets::CLEAN))
            cmd.addArg(m_target_name);

        return cmd;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::projectTargets() -> QStringList {
        return static_cast<XMakeBuildSystem *>(buildSystem())->targetList();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::setBuildTarget(const QString &target_name) -> void {
        m_target_name = target_name;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::toMap() const -> QVariantMap {
        auto map = AbstractProcessStep::toMap();

        map[QString::fromLatin1(Constants::BuildStep::TARGETS_KEY)]        = m_target_name;
        map[QString::fromLatin1(Constants::BuildStep::TOOL_ARGUMENTS_KEY)] = m_command_args;

        return map;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::fromMap(const QVariantMap &map) -> bool {
        m_target_name =
            map.value(QString::fromLatin1(Constants::BuildStep::TARGETS_KEY)).toString();
        m_command_args =
            map.value(QString::fromLatin1(Constants::BuildStep::TOOL_ARGUMENTS_KEY)).toString();

        return AbstractProcessStep::fromMap(map);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::update(bool parsing_successful) -> void {
        if (parsing_successful) {
            if (!projectTargets().contains(m_target_name)) setBuildTarget(defaultBuildTarget());

            Q_EMIT targetListChanged();
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::doRun() -> void {
        AbstractProcessStep::doRun();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::setupOutputFormatter(Utils::OutputFormatter *formatter) -> void {
        auto xmake_output_parser = new XMakeOutputParser {};
        xmake_output_parser->setSourceDirectory(project()->projectDirectory());

        formatter->addLineParser(xmake_output_parser); // formatter get ownship of parser

        const auto toolchain = QtSupport::CppKitInfo { kit() }.cxxToolChain;
        m_xmake_parser       = new XMakeBuildParser { (toolchain->displayName().contains("visual")
                                                           ? XMakeBuildParser::Type::MSVC
                                                           : XMakeBuildParser::Type::GCC_Clang) };
        m_xmake_parser->setSourceDirectory(project()->projectDirectory());

        formatter->addLineParser(m_xmake_parser);

        auto additional_parsers = kit()->createOutputParsers();

        for (auto *parser : additional_parsers) parser->setRedirectionDetector(m_xmake_parser);

        formatter->addLineParsers(additional_parsers);
        formatter->addSearchDir(processParameters()->effectiveWorkingDirectory());

        AbstractProcessStep::setupOutputFormatter(formatter);

        connect(m_xmake_parser, &XMakeBuildParser::reportProgress, this, [this](auto percent) {
            Q_EMIT progress(percent, QString {});
        });
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    auto XMakeBuildStep::defaultBuildTarget() const -> QString {
        const auto *bsl = stepList();
        QTC_ASSERT(bsl, return {});

        const auto parent_id = bsl->id();

        if (parent_id == ProjectExplorer::Constants::BUILDSTEPS_CLEAN)
            return QString::fromLatin1(Constants::Targets::CLEAN);

        return QString::fromLatin1(Constants::Targets::ALL);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildStepFactory::XMakeBuildStepFactory() {
        registerStep<XMakeBuildStep>(Constants::XMAKE_BUILD_STEP_ID);
        setSupportedProjectType(Constants::Project::ID);
        setDisplayName(XMakeBuildStep::tr("XMake build"));
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    XMakeBuildStepFactory::~XMakeBuildStepFactory() = default;
} // namespace XMakeProjectManager::Internal

#pragma once

#include <projectexplorer/abstractprocessstep.h>
#include <projectexplorer/buildstep.h>

namespace XMakeProjectManager::Internal {
    class XMakeBuildParser;
    class XMakeBuildStep final: public ProjectExplorer::AbstractProcessStep {
        Q_OBJECT
      public:
        XMakeBuildStep(ProjectExplorer::BuildStepList *bsl, Utils::Id id);

        QWidget *createConfigWidget() override;

        Utils::CommandLine command();

        QStringList projectTargets();

        void setBuildTarget(const QString &target_name);

        void setCommandArgs(const QString &args);

        const QString &targetName() const;

        QVariantMap toMap() const override;
        bool fromMap(const QVariantMap &map) override;

      Q_SIGNALS:
        void targetListChanged();
        void commandChanged();

      private:
        void update(bool parsing_successful);
        void doRun() override;
        void setupOutputFormatter(Utils::OutputFormatter *formatter) override;
        QString defaultBuildTarget() const;

        QString m_command_args;
        QString m_target_name;
        XMakeBuildParser *m_xmake_parser;
    };

    class XMakeBuildStepFactory final: public ProjectExplorer::BuildStepFactory {
      public:
        XMakeBuildStepFactory();
        ~XMakeBuildStepFactory();

        XMakeBuildStepFactory(XMakeBuildStepFactory &&)      = delete;
        XMakeBuildStepFactory(const XMakeBuildStepFactory &) = delete;

        XMakeBuildStepFactory &operator=(XMakeBuildStepFactory &&) = delete;
        XMakeBuildStepFactory &operator=(const XMakeBuildStepFactory &&) const = delete;
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeBuildStep.inl"

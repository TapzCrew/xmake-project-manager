#pragma once

#include <projectexplorer/buildconfiguration.h>

#include <QString>
#include <QStringView>

namespace XMakeProjectManager::Internal {
    enum class XMakeBuildType {
        Debug,
        Release,
        Release_Dbg,
        Min_Size_Rel,
        Check,
        Profile,
        Coverage,
        Valgrind,
        Asan,
        Tsan,
        Lsan,
        Ubsan
    };

    QString xmakeBuildTypeName(XMakeBuildType type);

    QString xmakeBuildTypeDisplayName(XMakeBuildType type);

    XMakeBuildType xmakeBuildType(QStringView type_name);

    ProjectExplorer::BuildConfiguration::BuildType buildType(XMakeBuildType type);

    class XMakeBuildSystem;
    class XMakeTools;
    class XMakeBuildConfiguration final: public ProjectExplorer::BuildConfiguration {
        Q_OBJECT
      public:
        XMakeBuildConfiguration(ProjectExplorer::Target *target, Utils::Id id);

        static Utils::FilePath
            shadowBuildDirectory(const Utils::FilePath &project_file_path,
                                 const ProjectExplorer::Kit *kit,
                                 const QString &bc_name,
                                 ProjectExplorer::BuildConfiguration::BuildType build_type);

        ProjectExplorer::BuildSystem *buildSystem() const override;

        void build(const QString &target);

        QStringList xmakeConfigArgs();

        QStringView parameters() const noexcept;

        void setParameters(QString params);

      Q_SIGNALS:
        void parametersChanged();
        void environmentChanged();

      private:
        QVariantMap toMap() const override;

        bool fromMap(const QVariantMap &map) override;

        XMakeBuildType m_build_type;

        ProjectExplorer::NamedWidget *createConfigWidget() override;

        std::unique_ptr<XMakeBuildSystem> m_build_system;

        QString m_parameters;
    };

    class XMakeBuildConfigurationFactory final: public ProjectExplorer::BuildConfigurationFactory {
      public:
        XMakeBuildConfigurationFactory();
    };
} // namespace XMakeProjectManager::Internal

#include "XMakeBuildConfiguration.inl"

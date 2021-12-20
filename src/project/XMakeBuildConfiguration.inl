#pragma once

#include "XMakeBuildConfiguration.hpp"

namespace XMakeProjectManager::Internal {
    static inline const QHash<QString, XMakeBuildType> BUILD_TYPES_BY_NAME = {
        { QString { "plain" }, XMakeBuildType::Plain },
        { QString { "debug" }, XMakeBuildType::Debug },
        { QString { "release" }, XMakeBuildType::Release }
    };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto xmakeBuildTypeName(XMakeBuildType type) -> QString {
        return BUILD_TYPES_BY_NAME.key(type, "plain");
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto xmakeBuildTypeDisplayName(XMakeBuildType type) -> QString {
        switch (type) {
            case XMakeBuildType::Debug: return { "Debug" };
            case XMakeBuildType::Release: return { "Release" };
            default: return { "Plain" };
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto xmakeBuildType(const QString &type_name) -> XMakeBuildType {
        return BUILD_TYPES_BY_NAME.value(type_name, XMakeBuildType::Plain);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto buildType(XMakeBuildType type) -> ProjectExplorer::BuildConfiguration::BuildType {
        switch (type) {
            case XMakeBuildType::Debug: return ProjectExplorer::BuildConfiguration::Debug;
            case XMakeBuildType::Release: return ProjectExplorer::BuildConfiguration::Release;
            default: return ProjectExplorer::BuildConfiguration::Unknown;
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildConfiguration::parameters() const noexcept -> QStringView {
        return m_parameters;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildConfiguration::setParameters(QString params) -> void {
        m_parameters = params;
    }
} // namespace XMakeProjectManager::Internal

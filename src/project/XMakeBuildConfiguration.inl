// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace XMakeProjectManager::Internal {
    static inline const QHash<QString, XMakeBuildType> BUILD_TYPES_BY_NAME = {
        { QString { "debug" }, XMakeBuildType::Debug },
        { QString { "release" }, XMakeBuildType::Release },
        { QString { "releasedbg" }, XMakeBuildType::Release_Dbg },
        { QString { "minsizerel" }, XMakeBuildType::Min_Size_Rel },
        { QString { "check" }, XMakeBuildType::Check },
        { QString { "profile" }, XMakeBuildType::Profile },
        { QString { "coverage" }, XMakeBuildType::Coverage },
        { QString { "valgrind" }, XMakeBuildType::Valgrind },
        { QString { "asan" }, XMakeBuildType::Asan },
        { QString { "tsan" }, XMakeBuildType::Tsan },
        { QString { "lsan" }, XMakeBuildType::Lsan },
        { QString { "ubsan" }, XMakeBuildType::Ubsan },
    };

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto xmakeBuildTypeName(XMakeBuildType type) -> QString {
        return BUILD_TYPES_BY_NAME.key(type, "release");
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto xmakeBuildTypeDisplayName(XMakeBuildType type) -> QString {
        switch (type) {
            case XMakeBuildType::Debug: return { "Debug" };
            case XMakeBuildType::Release: return { "Release" };
            case XMakeBuildType::Release_Dbg: return { "Release_Dbg" };
            case XMakeBuildType::Min_Size_Rel: return { "Min_Size_Rel" };
            case XMakeBuildType::Check: return { "Check" };
            case XMakeBuildType::Profile: return { "Profile" };
            case XMakeBuildType::Coverage: return { "Coverage" };
            case XMakeBuildType::Valgrind: return { "Valgrind" };
            case XMakeBuildType::Asan: return { "Asan" };
            case XMakeBuildType::Tsan: return { "Tsan" };
            case XMakeBuildType::Lsan: return { "Lsan" };
            case XMakeBuildType::Ubsan: return { "Ubsan" };
        }

        return "Unknown";
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto xmakeBuildType(const QString &type_name) -> XMakeBuildType {
        return BUILD_TYPES_BY_NAME.value(type_name, XMakeBuildType::Release);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto buildType(XMakeBuildType type) -> ProjectExplorer::BuildConfiguration::BuildType {
        switch (type) {
            case XMakeBuildType::Debug: return ProjectExplorer::BuildConfiguration::Debug;
            case XMakeBuildType::Check: return ProjectExplorer::BuildConfiguration::Debug;
            case XMakeBuildType::Profile: return ProjectExplorer::BuildConfiguration::Profile;
            default: return ProjectExplorer::BuildConfiguration::Release;
        }
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildConfiguration::parameters() const noexcept -> QStringView {
        return m_parameters;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildConfiguration::setParameters(const QString &params) -> void {
        m_parameters = params;
        Q_EMIT parametersChanged();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildConfiguration::addParameters(const QString &params) -> void {
        m_parameters += " ";
        m_parameters += params;
        Q_EMIT parametersChanged();
    }
} // namespace XMakeProjectManager::Internal

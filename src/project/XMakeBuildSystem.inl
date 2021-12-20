#pragma once

#include "XMakeBuildSystem.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildSystem::name() const -> QString { return QStringLiteral("XMake"); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildSystem::buildOptionsList() const noexcept -> const BuildOptionsList & {
        return m_build_options;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildSystem::targets() const noexcept -> const TargetsList & {
        return m_parser.targets();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildSystem::targetList() const noexcept -> const QStringList & {
        return m_parser.targetsNames();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    // inline auto XMakeBuildSystem::setXMakeConfigArgs(QStringList args) {
    //    m_pending_config_args = std::move(args);
    //}
} // namespace XMakeProjectManager::Internal

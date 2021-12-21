#pragma once

#include "XMakeBuildStep.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildStep::setCommandArgs(const QString &args) -> void {
        m_command_args = args.trimmed();
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeBuildStep::targetName() const -> const QString & { return m_target_name; }
} // namespace XMakeProjectManager::Internal

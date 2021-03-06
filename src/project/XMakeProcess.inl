// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProcess::state() const -> QProcess::ProcessState { return m_process->state(); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProcess::reportCanceled() -> void { m_future.reportCanceled(); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProcess::reportFinished() -> void { m_future.reportFinished(); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProcess::setProgressValue(int p) -> void { m_future.setProgressValue(p); }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProcess::stdOut() const noexcept -> const QByteArray & { return m_stdo; }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProcess::currentCommand() const noexcept -> const Command & {
        return m_current_command;
    }
} // namespace XMakeProjectManager::Internal

#pragma once

#include "XMakeProjectParser.hpp"

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProjectParser::setXMakeTool(const Utils::Id &xmake) noexcept -> void {
        m_xmake = xmake;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProjectParser::takeProjectNode() noexcept
        -> std::unique_ptr<XMakeProjectNode> {
        return std::move(m_root_node);
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProjectParser::targets() const noexcept -> const TargetsList & {
        return m_parser_result.targets;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProjectParser::targetsNames() const noexcept -> const QStringList & {
        return m_targets_names;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProjectParser::setEnvironment(const Utils::Environment &env) -> void {
        m_env = env;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProjectParser::srcDir() const noexcept -> const Utils::FilePath & {
        return m_src_dir;
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto XMakeProjectParser::buildDir() const noexcept -> const Utils::FilePath & {
        return m_build_dir;
    }
} // namespace XMakeProjectManager::Internal

// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto OptionParser::options() const noexcept -> BuildOptionsList {
        auto out = BuildOptionsList {};
        out.reserve(std::size(m_options));

        std::transform(std::begin(m_options),
                       std::end(m_options),
                       std::back_inserter(out),
                       [](const auto &o) { return std::make_unique<BuildOption>(*o); });

        return out;
    }
} // namespace XMakeProjectManager::Internal

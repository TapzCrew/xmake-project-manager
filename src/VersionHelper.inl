// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include "VersionHelper.hpp"

namespace XMakeProjectManager::Internal {
    inline Version::Version() = default;

    inline Version::Version(int major, int minor, int patch)
        : m_major { major }, m_minor { minor }, m_patch { patch }, m_is_valid { m_major != -1 &&
                                                                                m_minor != -1 &&
                                                                                m_patch != -1 } {}

    inline auto Version::toQString() const noexcept -> QString {
        return QString { QStringLiteral("%1.%2.%3") }.arg(m_major, m_minor, m_patch);
    }

    inline auto Version::fromString(QStringView str) -> Version {
        static const auto regex = QRegularExpression { R"((\d+).(\d+).(\d+))" };

        const auto matched = regex.match(str);

        if (matched.hasMatch())
            return Version { matched.captured(1).toInt(),
                             matched.captured(2).toInt(),
                             matched.captured(3).toInt() };

        return Version {};
    }

    inline auto Version::major() const noexcept -> int { return m_major; }
    inline auto Version::minor() const noexcept -> int { return m_minor; }
    inline auto Version::patch() const noexcept -> int { return m_patch; }
    inline auto Version::valid() const noexcept -> bool { return m_is_valid; }
} // namespace XMakeProjectManager::Internal

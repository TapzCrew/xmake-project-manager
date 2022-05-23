// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <QRegularExpression>
#include <QStringView>

namespace XMakeProjectManager::Internal {
    class Version {
      public:
        Version();
        Version(int major, int minor, int patch);

        QString toQString() const noexcept;
        static Version fromString(QStringView str);

        int major() const noexcept;
        int minor() const noexcept;
        int patch() const noexcept;
        bool valid() const noexcept;

      private:
        int m_major = -1;
        int m_minor = -1;
        int m_patch = -1;

        bool m_is_valid = false;
    };
} // namespace XMakeProjectManager::Internal

#include "VersionHelper.inl"

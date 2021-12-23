#pragma once

#include <QRegularExpression>
#include <QStringView>

namespace XMakeProjectManager::Internal {
    class Version {
      public:
        Version();
        Version(int major, int minor, int patch);

        QString toQString() const noexcept;
        static Version fromString(const QString& str);

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

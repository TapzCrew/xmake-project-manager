// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

namespace XMakeProjectManager::Internal {
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline BuildOption::BuildOption(QString _name,
                                    QString _description,
                                    QString _value,
                                    QStringList _values)
        : name { _name }, description { _description }, value { _value }, values { _values } {
    }

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    inline auto BuildOption::xmakeArg() const noexcept -> QString {
        return QString { "--%1=%2" }.arg(name).arg(value);
    }
} // namespace XMakeProjectManager::Internal

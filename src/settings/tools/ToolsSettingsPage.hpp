// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <coreplugin/dialogs/ioptionspage.h>

#include <QCoreApplication>

namespace XMakeProjectManager::Internal {
    class ToolsSettingsPage final: public Core::IOptionsPage {
        Q_DECLARE_TR_FUNCTIONS(XMakeProjectManager::Internal::ToolsSettingsPage)

      public:
        ToolsSettingsPage();
    };
} // namespace XMakeProjectManager::Internal

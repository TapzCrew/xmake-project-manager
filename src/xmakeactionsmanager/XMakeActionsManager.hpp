// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <utils/parameteraction.h>

namespace XMakeProjectManager::Internal {
    class XMakeActionsManager: public QObject {
        Q_OBJECT

      public:
        XMakeActionsManager();

      private:
        void configureCurrentProject();
        void updateContextActions();

        QAction m_configure_action_context_menu;
        QAction m_configure_action_menu;
        Utils::ParameterAction m_build_target_context_action;
    };
} // namespace XMakeProjectManager::Internal

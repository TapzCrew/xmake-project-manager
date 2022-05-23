// Copyright (C) 2022 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#pragma once

#include <qplatformdefs.h>

namespace XMakeProjectManager::Constants {
    namespace Icons {
        static constexpr auto XMAKE              = ":/xmakeproject/icons/xmake_logo.png";
        static constexpr auto XMAKE_FILE_OVERLAY = ":/xmakeproject/icons/xmake_file_overlay.png";
        static constexpr auto XMAKE_BW           = ":/xmakeproject/icons/xmake_bw_logo.png";
    } // namespace Icons

    namespace Project {
        static constexpr auto MIMETYPE = "text/x-xmake-project";
        static constexpr auto ID       = "XMakeProjectManager.XMakeProject";
    } // namespace Project

    namespace SettingsPage {
        static constexpr auto GENERAL_ID = "A.XMakeProjectManager.SettingsPage.General";
        static constexpr auto TOOLS_ID   = "Z.XMakeProjectManager.SettingsPage.Tools";
        static constexpr auto CATEGORY   = "Z.XMake";
    } // namespace SettingsPage

    namespace ToolsSettings {
        static constexpr auto FILENAME         = "xmaketool.xml";
        static constexpr auto ENTRY_KEY        = "XMakeProjectManager.Tool.";
        static constexpr auto ENTRY_COUNT      = "XMakeProjectManager.Tools.Count";
        static constexpr auto EXE_KEY          = "XMakeProjectManager.Tool.Exe";
        static constexpr auto AUTODETECTED_KEY = "XMakeProjectManager.Tool.AutoDetected";
        static constexpr auto NAME_KEY         = "XMakeProjectManager.Tool.name";
        static constexpr auto UUID_KEY         = "XMakeProjectManager.Tool.uuid";
        static constexpr auto TOOL_TYPE_XMAKE  = "xmake";
        static constexpr auto TOOL_TYPE_XREPO  = "xrepo"; /* for the future */
        static constexpr auto TOOL_TYPE_KEY    = "XMakeProjectManager.Tool.Type";
        static constexpr auto AUTORUN_KEY      = "XMakeProjectManager.Tool.Autorun";
        static constexpr auto AUTO_ACCEPT_REQUESTS_KEY =
            "XMakeProjectManager.Tool.AutoAcceptRequests";
    } // namespace ToolsSettings

    namespace BuildConfiguration {
        static constexpr auto BUILD_TYPE_KEY = "XMakeProjectManager.BuildConfig.Type";
        static constexpr auto PARAMETERS_KEY = "XMakeProjectManager.BuildConfig.Parameters";
    } // namespace BuildConfiguration

    namespace BuildStep {
        static constexpr auto TARGETS_KEY = "XMakeProjectManager.BuildStep.BuildTargets";
        static constexpr auto TOOL_ARGUMENTS_KEY =
            "XMakeProjectManager.BuildStep.AdditionalArguments";
    } // namespace BuildStep

    namespace Targets {
        static constexpr auto ALL   = "all";
        static constexpr auto CLEAN = "clean";
    } // namespace Targets

    static constexpr auto XMAKE_INFO_DIR = ".xmake";

#if defined(Q_OS_WINDOWS)
    #if INTPTR_MAX == INT64_MAX
    static constexpr auto XMAKE_INTRO_CONF = "/windows/x64/xmake.conf";
    #else
    static constexpr auto XMAKE_INTRO_CONF = "/windows/x32/xmake.conf";
    #endif
#elif defined(Q_OS_LINUX)
    #if INTPTR_MAX == INT64_MAX
    static constexpr auto XMAKE_INTRO_CONF = "/linux/x64/xmake.conf";
    #else
    static constexpr auto XMAKE_INTRO_CONF = "/linux/x32/xmake.conf";
    #endif
#elif defined(Q_OS_MACOS)
    #if INTPTR_MAX == INT64_MAX
    static constexpr auto XMAKE_INTRO_CONF = "/macos/x64/xmake.conf";
    #else
    static constexpr auto XMAKE_INTRO_CONF = "/macos/x32/xmake.conf";
    #endif
#endif

    static constexpr auto TOOL_NAME = "xmake";
    static constexpr auto TOOL_ID   = "XMakeProjectManager.XMakeKitInformation.XMake";

    static constexpr auto XMAKE_TOOL_MANAGER    = "XMakeProjectManager.Tools";
    static constexpr auto XMAKE_BUILD_STEP_ID   = "XMakeProjectManager.BuildStep";
    static constexpr auto XMAKE_BUILD_CONFIG_ID = "XMakeProjectManager.BuildConfiguration";
} // namespace XMakeProjectManager::Constants

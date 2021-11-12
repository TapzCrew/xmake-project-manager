#pragma once

#include <qplatformdefs.h>

namespace XMakeProjectManager::Constants {
    namespace Icons {
        static constexpr auto XMAKE    = ":/xmakeproject/icons/xmake_logo.png";
        static constexpr auto XMAKE_BW = ":/xmakeproject/icons/xmake_bw_logo.png";
    } // namespace Icons

    namespace Project {
        static constexpr auto MIMETYPE = "text/x-xmake";
        static constexpr auto ID       = "XMakeProjectManager.XMakeProject";
    } // namespace Project

    namespace SettingsPage {
        static constexpr auto GENERAL_ID = "A.XMakeProjectManager.SettingsPage.General";
        static constexpr auto TOOLS_ID   = "Z.XMakeProjectManager.SettingsPage.Tools";
        static constexpr auto CATEGORY   = "Z.XMake";
    } // namespace SettingsPage

    namespace ToolsSettings {
        static constexpr auto FILENAME         = "xmaketool.xml";
        static constexpr auto ENTRY_KEY        = "Tool.";
        static constexpr auto ENTRY_COUNT      = "Tools.Count";
        static constexpr auto EXE_KEY          = "exe";
        static constexpr auto AUTODETECTED_KEY = "autodetected";
        static constexpr auto NAME_KEY         = "name";
        static constexpr auto UUID_KEY         = "uuid";
        static constexpr auto TOOL_TYPE_XMAKE  = "xmake";
        static constexpr auto TOOL_TYPE_XREPO  = "xrepo"; /* for the future */
        static constexpr auto TOOL_TYPE_KEY    = "type";
    } // namespace ToolsSettings

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

    static constexpr auto XMAKE_TOOL_MANAGER  = "XMakeProjectManager.Tools";
    static constexpr auto XMAKE_BUILD_STEP_ID = "XMakeProjectManager.BuildStep";
} // namespace XMakeProjectManager::Constants

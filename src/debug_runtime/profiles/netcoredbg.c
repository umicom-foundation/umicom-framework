/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/netcoredbg.c
 *
 * PURPOSE:
 *   Define the Framework-owned NetCoreDbg external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/netcoredbg.h"

/*
 * Provide the debug runtime profile netcoredbg operation used by this module and its
 * client applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_netcoredbg(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.netcoredbg",
        .display_name = "NetCoreDbg",
        .executable = "netcoredbg",
        .arguments = "--interpreter=vscode",
        .debugger_kind = "dotnet",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

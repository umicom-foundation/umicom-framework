/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/netcoredbg.c
 *
 * PURPOSE:
 *   Define the Framework-owned NetCoreDbg external adapter profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/netcoredbg.h"

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

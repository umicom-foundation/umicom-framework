/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/cppvsdbg.c
 *
 * PURPOSE:
 *   Define the Framework-owned Visual Studio C++ Debug Adapter external adapter profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/cppvsdbg.h"

const UmiDebugAdapterProfile *umi_debug_runtime_profile_cppvsdbg(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.cppvsdbg",
        .display_name = "Visual Studio C++ Debug Adapter",
        .executable = "OpenDebugAD7.exe",
        .arguments = "",
        .debugger_kind = "cppvsdbg",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

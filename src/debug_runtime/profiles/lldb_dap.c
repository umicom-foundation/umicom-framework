/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/lldb_dap.c
 *
 * PURPOSE:
 *   Define the Framework-owned LLDB DAP external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/lldb_dap.h"

/*
 * Provide the debug runtime profile lldb dap operation used by this module and its client
 * applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_lldb_dap(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.lldb-dap",
        .display_name = "LLDB DAP",
        .executable = "lldb-dap",
        .arguments = "",
        .debugger_kind = "lldb",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

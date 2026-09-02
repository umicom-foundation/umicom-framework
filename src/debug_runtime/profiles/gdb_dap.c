/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/gdb_dap.c
 *
 * PURPOSE:
 *   Define the Framework-owned GDB DAP external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/gdb_dap.h"

/*
 * Provide the debug runtime profile gdb dap operation used by this module and its client
 * applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_gdb_dap(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.gdb-dap",
        .display_name = "GDB DAP",
        .executable = "gdb",
        .arguments = "-i=dap",
        .debugger_kind = "gdb",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

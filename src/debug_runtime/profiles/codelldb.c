/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/codelldb.c
 *
 * PURPOSE:
 *   Define the Framework-owned CodeLLDB external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/codelldb.h"

/*
 * Provide the debug runtime profile codelldb operation used by this module and its client
 * applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_codelldb(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.codelldb",
        .display_name = "CodeLLDB",
        .executable = "codelldb",
        .arguments = "",
        .debugger_kind = "lldb",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

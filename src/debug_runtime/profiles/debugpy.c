/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/debugpy.c
 *
 * PURPOSE:
 *   Define the Framework-owned Python debugpy external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/debugpy.h"

/*
 * Provide the debug runtime profile debugpy operation used by this module and its client
 * applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_debugpy(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.debugpy",
        .display_name = "Python debugpy",
        .executable = "python",
        .arguments = "-m debugpy.adapter",
        .debugger_kind = "python",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

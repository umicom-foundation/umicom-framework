/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/debugpy.c
 *
 * PURPOSE:
 *   Define the Framework-owned Python debugpy external adapter profile.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/debugpy.h"

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

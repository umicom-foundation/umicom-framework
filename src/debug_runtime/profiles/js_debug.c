/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/js_debug.c
 *
 * PURPOSE:
 *   Define the Framework-owned JavaScript Debug Adapter external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/js_debug.h"

/*
 * Provide the debug runtime profile js debug operation used by this module and its client
 * applications.
 */
const UmiDebugAdapterProfile *umi_debug_runtime_profile_js_debug(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.js-debug",
        .display_name = "JavaScript Debug Adapter",
        .executable = "js-debug-adapter",
        .arguments = "",
        .debugger_kind = "javascript",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

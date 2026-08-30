/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/php_debug.c
 *
 * PURPOSE:
 *   Define the Framework-owned PHP Debug Adapter external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/php_debug.h"

const UmiDebugAdapterProfile *umi_debug_runtime_profile_php_debug(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.php-debug",
        .display_name = "PHP Debug Adapter",
        .executable = "php-debug-adapter",
        .arguments = "",
        .debugger_kind = "php",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

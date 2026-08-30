/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/profiles/java_debug.c
 *
 * PURPOSE:
 *   Define the Framework-owned Java Debug Adapter external adapter profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/profiles/java_debug.h"

const UmiDebugAdapterProfile *umi_debug_runtime_profile_java_debug(void)
{
    static const UmiDebugAdapterProfile profile = {
        .id = "debug.adapter.java-debug",
        .display_name = "Java Debug Adapter",
        .executable = "java-debug-adapter",
        .arguments = "",
        .debugger_kind = "java",
        .supports_launch = 1,
        .supports_attach = 1,
        .enabled = 1,
        .revision = 1U
    };
    return &profile;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_command_router.c
 *
 * PURPOSE:
 *   Verify command context defaults and Start command readiness without
 *   launching an external adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/command_router.h"

int main(void)
{
    UmiDebugRuntimePlatform *platform = NULL;
    UmiDebugRuntimeCommandContext context;

    assert(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    umi_debug_runtime_command_context_init(&context);

    assert(!umi_debug_runtime_command_enabled(
        platform, UMI_DEBUG_COMMAND_START, &context));

    (void)strcpy(context.profile_id, "debug.adapter.gdb-dap");
    (void)strcpy(context.session_id, "debug.1");
    (void)strcpy(context.configuration_id, "config.1");

    assert(umi_debug_runtime_command_enabled(
        platform, UMI_DEBUG_COMMAND_START, &context));

    umi_debug_runtime_platform_destroy(platform);
    return 0;
}

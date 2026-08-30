/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_platform_create.c
 *
 * PURPOSE:
 *   Verify the operational runtime composes existing Debug Service and Advanced
 *   Debugging without starting any external adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/debug_runtime/platform.h"

int main(void)
{
    UmiDebugRuntimePlatform *platform = NULL;
    UmiDebugRuntimePlatformSnapshot snapshot;

    assert(umi_debug_runtime_platform_create(&platform) == UMI_STATUS_OK);
    assert(umi_debug_runtime_platform_snapshot(
        platform, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.active == 0);
    assert(snapshot.service.adapter_profile_count >= 9U);

    umi_debug_runtime_platform_destroy(platform);
    return 0;
}

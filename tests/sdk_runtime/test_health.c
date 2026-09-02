/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_health.c
 *
 * PURPOSE:
 *   Verify the health contract and revision behaviour.
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
#include "umicom/sdk_runtime/health.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeHealth value; UmiSdkRuntimeHealth same; uint64_t revision;
    umi_sdk_runtime_health_init(&value, "sdk-runtime.health");
    assert(umi_sdk_runtime_health_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_health_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_health_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_health_set_ready_components(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_health_set_blocked_components(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_health_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.ready_components == 3U && value.blocked_components == 5U);
    umi_sdk_runtime_health_init(&same, "sdk-runtime.health");
    assert(umi_sdk_runtime_health_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

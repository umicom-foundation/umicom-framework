/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_service.c
 *
 * PURPOSE:
 *   Verify the service contract and revision behaviour.
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
#include "umicom/sdk_runtime/service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeService value; UmiSdkRuntimeService same; uint64_t revision;
    umi_sdk_runtime_service_init(&value, "sdk-runtime.service");
    assert(umi_sdk_runtime_service_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_service_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_service_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_service_set_lifecycle_state_code(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_service_set_generation(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_service_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.lifecycle_state_code == 3U && value.generation == 5U);
    umi_sdk_runtime_service_init(&same, "sdk-runtime.service");
    assert(umi_sdk_runtime_service_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

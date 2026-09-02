/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_controller.c
 *
 * PURPOSE:
 *   Verify the controller contract and revision behaviour.
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
#include "umicom/sdk_runtime/controller.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeController value; UmiSdkRuntimeController same; uint64_t revision;
    umi_sdk_runtime_controller_init(&value, "sdk-runtime.controller");
    assert(umi_sdk_runtime_controller_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_controller_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_controller_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_controller_set_lifecycle_state_code(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_controller_set_command_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_controller_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.lifecycle_state_code == 3U && value.command_count == 5U);
    umi_sdk_runtime_controller_init(&same, "sdk-runtime.controller");
    assert(umi_sdk_runtime_controller_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

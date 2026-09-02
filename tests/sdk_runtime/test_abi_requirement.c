/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_abi_requirement.c
 *
 * PURPOSE:
 *   Verify the abi requirement contract and revision behaviour.
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
#include "umicom/sdk_runtime/abi_requirement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSdkRuntimeAbiRequirement value; UmiSdkRuntimeAbiRequirement same; uint64_t revision;
    umi_sdk_runtime_abi_requirement_init(&value, "sdk-runtime.abi_requirement");
    assert(umi_sdk_runtime_abi_requirement_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_abi_requirement_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_abi_requirement_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_abi_requirement_set_minimum_abi(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_abi_requirement_set_maximum_abi(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_abi_requirement_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.minimum_abi == 3U && value.maximum_abi == 5U);
    umi_sdk_runtime_abi_requirement_init(&same, "sdk-runtime.abi_requirement");
    assert(umi_sdk_runtime_abi_requirement_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

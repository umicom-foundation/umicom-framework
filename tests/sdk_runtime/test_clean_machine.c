/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_clean_machine.c
 *
 * PURPOSE:
 *   Verify the clean machine contract and revision behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/sdk_runtime/clean_machine.h"
int main(void)
{
    UmiSdkRuntimeCleanMachine value; UmiSdkRuntimeCleanMachine same; uint64_t revision;
    umi_sdk_runtime_clean_machine_init(&value, "sdk-runtime.clean_machine");
    assert(umi_sdk_runtime_clean_machine_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_clean_machine_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_clean_machine_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_clean_machine_set_requirement_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_clean_machine_set_missing_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_clean_machine_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.requirement_count == 3U && value.missing_count == 5U);
    umi_sdk_runtime_clean_machine_init(&same, "sdk-runtime.clean_machine");
    assert(umi_sdk_runtime_clean_machine_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

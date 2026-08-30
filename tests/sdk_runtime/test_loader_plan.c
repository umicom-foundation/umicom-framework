/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_loader_plan.c
 *
 * PURPOSE:
 *   Verify the loader plan contract and revision behaviour.
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
#include "umicom/sdk_runtime/loader_plan.h"
int main(void)
{
    UmiSdkRuntimeLoaderPlan value; UmiSdkRuntimeLoaderPlan same; uint64_t revision;
    umi_sdk_runtime_loader_plan_init(&value, "sdk-runtime.loader_plan");
    assert(umi_sdk_runtime_loader_plan_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_loader_plan_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_plan_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_plan_set_entry_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_plan_set_generation(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_loader_plan_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.entry_count == 3U && value.generation == 5U);
    umi_sdk_runtime_loader_plan_init(&same, "sdk-runtime.loader_plan");
    assert(umi_sdk_runtime_loader_plan_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

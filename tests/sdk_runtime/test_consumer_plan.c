/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_consumer_plan.c
 *
 * PURPOSE:
 *   Verify the consumer plan contract and revision behaviour.
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
#include "umicom/sdk_runtime/consumer_plan.h"
int main(void)
{
    UmiSdkRuntimeConsumerPlan value; UmiSdkRuntimeConsumerPlan same; uint64_t revision;
    umi_sdk_runtime_consumer_plan_init(&value, "sdk-runtime.consumer_plan");
    assert(umi_sdk_runtime_consumer_plan_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_consumer_plan_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_plan_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_plan_set_compile_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_plan_set_runtime_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_consumer_plan_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.compile_count == 3U && value.runtime_count == 5U);
    umi_sdk_runtime_consumer_plan_init(&same, "sdk-runtime.consumer_plan");
    assert(umi_sdk_runtime_consumer_plan_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

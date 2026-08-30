/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/sdk_runtime/test_metrics.c
 *
 * PURPOSE:
 *   Verify the metrics contract and revision behaviour.
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
#include "umicom/sdk_runtime/metrics.h"
int main(void)
{
    UmiSdkRuntimeMetrics value; UmiSdkRuntimeMetrics same; uint64_t revision;
    umi_sdk_runtime_metrics_init(&value, "sdk-runtime.metrics");
    assert(umi_sdk_runtime_metrics_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_sdk_runtime_metrics_set_path(&value, "share/umicom/runtime") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_metrics_set_detail(&value, "validated runtime evidence") == UMI_STATUS_OK);
    assert(umi_sdk_runtime_metrics_set_probe_count(&value, 3U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_metrics_set_failure_count(&value, 5U) == UMI_STATUS_OK);
    assert(umi_sdk_runtime_metrics_set_state(&value, UMI_SDK_RUNTIME_STATE_READY) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.probe_count == 3U && value.failure_count == 5U);
    umi_sdk_runtime_metrics_init(&same, "sdk-runtime.metrics");
    assert(umi_sdk_runtime_metrics_same_identity(&value, &same));
    assert(strcmp(value.path, "share/umicom/runtime") == 0);
    return 0;
}

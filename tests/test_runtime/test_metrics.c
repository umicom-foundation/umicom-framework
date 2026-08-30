/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_metrics.c
 *
 * PURPOSE:
 *   Verify the metrics contract, bounded text and revision behaviour.
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
#include "umicom/test_runtime/metrics.h"

int main(void)
{
    UmiTestRuntimeMetrics value;
    UmiTestRuntimeMetrics same;
    uint64_t revision;
    umi_test_runtime_metrics_init(&value, "test-runtime.metrics");
    assert(value.structure_size == sizeof(value));
    assert(value.enabled);
    assert(umi_test_runtime_metrics_validate(&value) == UMI_STATUS_OK);
    revision = value.revision;
    assert(umi_test_runtime_metrics_set_name(&value, "Regression Runtime") == UMI_STATUS_OK);
    assert(umi_test_runtime_metrics_set_detail(&value, "deterministic evidence") == UMI_STATUS_OK);
    assert(umi_test_runtime_metrics_set_launched(&value, 7U) == UMI_STATUS_OK);
    assert(umi_test_runtime_metrics_set_passed(&value, 11U) == UMI_STATUS_OK);
    assert(umi_test_runtime_metrics_touch(&value, 1234U) == UMI_STATUS_OK);
    assert(value.revision > revision);
    assert(value.launched == 7U);
    assert(value.passed == 11U);
    assert(strcmp(value.name, "Regression Runtime") == 0);
    umi_test_runtime_metrics_init(&same, "test-runtime.metrics");
    assert(umi_test_runtime_metrics_same_identity(&value, &same));
    assert(umi_test_runtime_metrics_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}

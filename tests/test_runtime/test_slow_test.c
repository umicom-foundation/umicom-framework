/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_slow_test.c
 *
 * PURPOSE:
 *   Verify the slow test runtime contract.
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
#include "umicom/test_runtime/slow_test.h"
int main(void)
{
    UmiTestRuntimeSlowTest value,same;
    uint64_t r;
    umi_test_runtime_slow_test_init(&value,"test-runtime.slow_test");
    assert(umi_test_runtime_slow_test_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_slow_test_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_slow_test_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_slow_test_set_duration_ms(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_slow_test_set_threshold_ms(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_slow_test_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.duration_ms==13U&&value.threshold_ms==21U);
    umi_test_runtime_slow_test_init(&same,"test-runtime.slow_test");
    assert(umi_test_runtime_slow_test_same_identity(&value,&same));
    return 0;
    }

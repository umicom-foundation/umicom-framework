/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_trend.c
 *
 * PURPOSE:
 *   Verify the trend runtime contract.
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
#include "umicom/test_runtime/trend.h"
int main(void)
{
    UmiTestRuntimeTrend value,same;
    uint64_t r;
    umi_test_runtime_trend_init(&value,"test-runtime.trend");
    assert(umi_test_runtime_trend_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_trend_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_trend_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_trend_set_sample_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_trend_set_generation(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_trend_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.sample_count==13U&&value.generation==21U);
    umi_test_runtime_trend_init(&same,"test-runtime.trend");
    assert(umi_test_runtime_trend_same_identity(&value,&same));
    return 0;
    }

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_execution_statistics.c
 *
 * PURPOSE:
 *   Verify the execution statistics runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/execution_statistics.h"
int main(void)
{
    UmiTestRuntimeExecutionStatistics value,same;
    uint64_t r;
    umi_test_runtime_execution_statistics_init(&value,"test-runtime.execution_statistics");
    assert(umi_test_runtime_execution_statistics_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_execution_statistics_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_statistics_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_statistics_set_sample_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_statistics_set_total_duration_ms(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_execution_statistics_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.sample_count==13U&&value.total_duration_ms==21U);
    umi_test_runtime_execution_statistics_init(&same,"test-runtime.execution_statistics");
    assert(umi_test_runtime_execution_statistics_same_identity(&value,&same));
    return 0;
    }

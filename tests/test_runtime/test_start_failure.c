/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_start_failure.c
 *
 * PURPOSE:
 *   Verify the start failure runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/start_failure.h"
int main(void)
{
    UmiTestRuntimeStartFailure value,same;
    uint64_t r;
    umi_test_runtime_start_failure_init(&value,"test-runtime.start_failure");
    assert(umi_test_runtime_start_failure_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_start_failure_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_start_failure_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_start_failure_set_native_status(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_start_failure_set_attempt_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_start_failure_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.native_status==13U&&value.attempt_count==21U);
    umi_test_runtime_start_failure_init(&same,"test-runtime.start_failure");
    assert(umi_test_runtime_start_failure_same_identity(&value,&same));
    return 0;
    }

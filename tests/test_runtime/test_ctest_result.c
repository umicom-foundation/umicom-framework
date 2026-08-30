/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_ctest_result.c
 *
 * PURPOSE:
 *   Verify the ctest result runtime contract.
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
#include "umicom/test_runtime/ctest_result.h"
int main(void)
{
    UmiTestRuntimeCtestResult value,same;
    uint64_t r;
    umi_test_runtime_ctest_result_init(&value,"test-runtime.ctest_result");
    assert(umi_test_runtime_ctest_result_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_ctest_result_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_result_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_result_set_duration_ms(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_result_set_native_status(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_result_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.duration_ms==13U&&value.native_status==21U);
    umi_test_runtime_ctest_result_init(&same,"test-runtime.ctest_result");
    assert(umi_test_runtime_ctest_result_same_identity(&value,&same));
    return 0;
    }

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_ctest_record.c
 *
 * PURPOSE:
 *   Verify the ctest record runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/ctest_record.h"
int main(void)
{
    UmiTestRuntimeCtestRecord value,same;
    uint64_t r;
    umi_test_runtime_ctest_record_init(&value,"test-runtime.ctest_record");
    assert(umi_test_runtime_ctest_record_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_ctest_record_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_record_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_record_set_processor_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_record_set_timeout_seconds(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_ctest_record_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.processor_count==13U&&value.timeout_seconds==21U);
    umi_test_runtime_ctest_record_init(&same,"test-runtime.ctest_record");
    assert(umi_test_runtime_ctest_record_same_identity(&value,&same));
    return 0;
    }

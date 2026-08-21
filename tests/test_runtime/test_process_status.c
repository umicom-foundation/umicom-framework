/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_process_status.c
 *
 * PURPOSE:
 *   Verify the process status runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/process_status.h"
int main(void)
{
    UmiTestRuntimeProcessStatus value,same;
    uint64_t r;
    umi_test_runtime_process_status_init(&value,"test-runtime.process_status");
    assert(umi_test_runtime_process_status_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_process_status_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_process_status_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_process_status_set_native_status(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_process_status_set_exit_code(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_process_status_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.native_status==13U&&value.exit_code==21U);
    umi_test_runtime_process_status_init(&same,"test-runtime.process_status");
    assert(umi_test_runtime_process_status_same_identity(&value,&same));
    return 0;
    }

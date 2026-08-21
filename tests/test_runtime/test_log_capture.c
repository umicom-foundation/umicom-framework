/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_log_capture.c
 *
 * PURPOSE:
 *   Verify the log capture runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/log_capture.h"
int main(void)
{
    UmiTestRuntimeLogCapture value,same;
    uint64_t r;
    umi_test_runtime_log_capture_init(&value,"test-runtime.log_capture");
    assert(umi_test_runtime_log_capture_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_log_capture_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_log_capture_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_log_capture_set_byte_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_log_capture_set_truncated(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_log_capture_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.byte_count==13U&&value.truncated==21U);
    umi_test_runtime_log_capture_init(&same,"test-runtime.log_capture");
    assert(umi_test_runtime_log_capture_same_identity(&value,&same));
    return 0;
    }

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_exit_interpretation.c
 *
 * PURPOSE:
 *   Verify the exit interpretation runtime contract.
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
#include "umicom/test_runtime/exit_interpretation.h"
int main(void)
{
    UmiTestRuntimeExitInterpretation value,same;
    uint64_t r;
    umi_test_runtime_exit_interpretation_init(&value,"test-runtime.exit_interpretation");
    assert(umi_test_runtime_exit_interpretation_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_exit_interpretation_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_exit_interpretation_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_exit_interpretation_set_exit_code(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_exit_interpretation_set_native_status(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_exit_interpretation_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.exit_code==13U&&value.native_status==21U);
    umi_test_runtime_exit_interpretation_init(&same,"test-runtime.exit_interpretation");
    assert(umi_test_runtime_exit_interpretation_same_identity(&value,&same));
    return 0;
    }

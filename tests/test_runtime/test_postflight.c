/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_postflight.c
 *
 * PURPOSE:
 *   Verify the postflight runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/postflight.h"
int main(void)
{
    UmiTestRuntimePostflight v,s;
    umi_test_runtime_postflight_init(&v,"test-runtime.postflight");
    assert(umi_test_runtime_postflight_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_postflight_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_postflight_set_check_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_postflight_set_failure_count(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_postflight_init(&s,"test-runtime.postflight");
    assert(umi_test_runtime_postflight_same_identity(&v,&s));
    return 0;
    }

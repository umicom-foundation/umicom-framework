/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_preflight.c
 *
 * PURPOSE:
 *   Verify the preflight runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/preflight.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimePreflight v,s;
    umi_test_runtime_preflight_init(&v,"test-runtime.preflight");
    assert(umi_test_runtime_preflight_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_preflight_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_preflight_set_check_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_preflight_set_failure_count(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_preflight_init(&s,"test-runtime.preflight");
    assert(umi_test_runtime_preflight_same_identity(&v,&s));
    return 0;
    }

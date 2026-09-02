/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_test_family.c
 *
 * PURPOSE:
 *   Verify the test family runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/test_family.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeTestFamily v,s;
    umi_test_runtime_test_family_init(&v,"test-runtime.test_family");
    assert(umi_test_runtime_test_family_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_test_family_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_test_family_set_member_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_test_family_set_generation(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_test_family_init(&s,"test-runtime.test_family");
    assert(umi_test_runtime_test_family_same_identity(&v,&s));
    return 0;
    }

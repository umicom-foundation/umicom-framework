/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_owner.c
 *
 * PURPOSE:
 *   Verify the failure owner runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/failure_owner.h"
int main(void)
{
    UmiTestRuntimeFailureOwner v,s;
    umi_test_runtime_failure_owner_init(&v,"test-runtime.failure_owner");
    assert(umi_test_runtime_failure_owner_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_owner_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_owner_set_priority(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_owner_set_generation(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_failure_owner_init(&s,"test-runtime.failure_owner");
    assert(umi_test_runtime_failure_owner_same_identity(&v,&s));
    return 0;
    }

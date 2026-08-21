/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_run_identity.c
 *
 * PURPOSE:
 *   Verify the run identity runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/run_identity.h"
int main(void)
{
    UmiTestRuntimeRunIdentity v,s;
    umi_test_runtime_run_identity_init(&v,"test-runtime.run_identity");
    assert(umi_test_runtime_run_identity_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_run_identity_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_run_identity_set_sequence(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_run_identity_set_generation(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_run_identity_init(&s,"test-runtime.run_identity");
    assert(umi_test_runtime_run_identity_same_identity(&v,&s));
    return 0;
    }

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_reproduction.c
 *
 * PURPOSE:
 *   Verify the failure reproduction runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/failure_reproduction.h"
int main(void)
{
    UmiTestRuntimeFailureReproduction v,s;
    umi_test_runtime_failure_reproduction_init(&v,"test-runtime.failure_reproduction");
    assert(umi_test_runtime_failure_reproduction_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_reproduction_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_reproduction_set_evidence_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_reproduction_set_generation(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_failure_reproduction_init(&s,"test-runtime.failure_reproduction");
    assert(umi_test_runtime_failure_reproduction_same_identity(&v,&s));
    return 0;
    }

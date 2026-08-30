/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_resolution_evidence.c
 *
 * PURPOSE:
 *   Verify the resolution evidence runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/resolution_evidence.h"
int main(void)
{
    UmiTestRuntimeResolutionEvidence v,s;
    umi_test_runtime_resolution_evidence_init(&v,"test-runtime.resolution_evidence");
    assert(umi_test_runtime_resolution_evidence_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_resolution_evidence_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_resolution_evidence_set_verification_count(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_resolution_evidence_set_generation(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_resolution_evidence_init(&s,"test-runtime.resolution_evidence");
    assert(umi_test_runtime_resolution_evidence_same_identity(&v,&s));
    return 0;
    }

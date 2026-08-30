/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_failure_fingerprint.c
 *
 * PURPOSE:
 *   Verify the failure fingerprint runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/test_runtime/failure_fingerprint.h"
int main(void)
{
    UmiTestRuntimeFailureFingerprint v,s;
    umi_test_runtime_failure_fingerprint_init(&v,"test-runtime.failure_fingerprint");
    assert(umi_test_runtime_failure_fingerprint_validate(&v)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_fingerprint_set_detail(&v,"regression evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_fingerprint_set_hash(&v,34U)==UMI_STATUS_OK);
    assert(umi_test_runtime_failure_fingerprint_set_occurrence_count(&v,55U)==UMI_STATUS_OK);
    umi_test_runtime_failure_fingerprint_init(&s,"test-runtime.failure_fingerprint");
    assert(umi_test_runtime_failure_fingerprint_same_identity(&v,&s));
    return 0;
    }

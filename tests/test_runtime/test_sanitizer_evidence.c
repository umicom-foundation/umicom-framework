/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_sanitizer_evidence.c
 *
 * PURPOSE:
 *   Verify the sanitizer evidence runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/test_runtime/sanitizer_evidence.h"
int main(void)
{
    UmiTestRuntimeSanitizerEvidence value,same;
    uint64_t r;
    umi_test_runtime_sanitizer_evidence_init(&value,"test-runtime.sanitizer_evidence");
    assert(umi_test_runtime_sanitizer_evidence_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_sanitizer_evidence_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_sanitizer_evidence_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_sanitizer_evidence_set_finding_count(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_sanitizer_evidence_set_suppressed_count(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_sanitizer_evidence_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.finding_count==13U&&value.suppressed_count==21U);
    umi_test_runtime_sanitizer_evidence_init(&same,"test-runtime.sanitizer_evidence");
    assert(umi_test_runtime_sanitizer_evidence_same_identity(&value,&same));
    return 0;
    }

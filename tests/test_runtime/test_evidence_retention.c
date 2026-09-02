/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_runtime/test_evidence_retention.c
 *
 * PURPOSE:
 *   Verify the evidence retention runtime contract.
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
#include "umicom/test_runtime/evidence_retention.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTestRuntimeEvidenceRetention value,same;
    uint64_t r;
    umi_test_runtime_evidence_retention_init(&value,"test-runtime.evidence_retention");
    assert(umi_test_runtime_evidence_retention_validate(&value)==UMI_STATUS_OK);
    r=value.revision;
    assert(umi_test_runtime_evidence_retention_set_category(&value,"regression")==UMI_STATUS_OK);
    assert(umi_test_runtime_evidence_retention_set_detail(&value,"evidence")==UMI_STATUS_OK);
    assert(umi_test_runtime_evidence_retention_set_retention_days(&value,13U)==UMI_STATUS_OK);
    assert(umi_test_runtime_evidence_retention_set_maximum_runs(&value,21U)==UMI_STATUS_OK);
    assert(umi_test_runtime_evidence_retention_set_active(&value,false)==UMI_STATUS_OK);
    assert(value.revision>r);
    assert(value.retention_days==13U&&value.maximum_runs==21U);
    umi_test_runtime_evidence_retention_init(&same,"test-runtime.evidence_retention");
    assert(umi_test_runtime_evidence_retention_same_identity(&value,&same));
    return 0;
    }

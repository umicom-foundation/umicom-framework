/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_toolchain_gate_blocked.c
 *
 * PURPOSE:
 *   Verify developer acceptance toolchain gate blocked behavior.
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
#include "umicom/developer/acceptance/toolchain_gate.h"
/*
 * Exercise test developer acceptance toolchain gate blocked and return a clear result when
 * the behaviour no longer matches its contract.
 */
int test_developer_acceptance_toolchain_gate_blocked(void)
{
    UmiDeveloperLanguageSupportSnapshot support={0};
    UmiDeveloperAcceptanceEvidenceStore store;
    const UmiDeveloperAcceptanceEvidence *evidence;
    strcpy(support.language_id,"python");
    strcpy(support.binding_id,"python.cpython");
    support.binding_found=1;
    support.supported_operation_count=7U;
    support.ready_operation_count=5U;
    support.missing_operation_count=2U;
    support.readiness_percent=71U;
    umi_developer_acceptance_evidence_store_init(&store);
    assert(umi_developer_acceptance_record_toolchain_support(&support,&store)==UMI_STATUS_OK);
    evidence=umi_developer_acceptance_evidence_find(&store,"umicom.developer.language.python");
    assert(evidence!=NULL&&evidence->state==UMI_DEVELOPER_ACCEPTANCE_BLOCKED);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_gate_blocked.c
 *
 * PURPOSE:
 *   Verify developer acceptance gate blocked behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/acceptance/gate.h"
int test_developer_acceptance_gate_blocked(void)
{
    UmiDeveloperAcceptanceEvidenceStore store;
    UmiDeveloperAcceptanceEvidence evidence={0};
    UmiDeveloperAcceptanceGate gate={0};
    UmiDeveloperAcceptanceGateResult result;
    umi_developer_acceptance_evidence_store_init(&store);
    strcpy(evidence.capability_id,"cap");
    evidence.state=UMI_DEVELOPER_ACCEPTANCE_BLOCKED;
    assert(umi_developer_acceptance_evidence_upsert(&store,&evidence)==UMI_STATUS_OK);
    strcpy(gate.gate_id,"gate");
    gate.required_capability_ids[0]="cap";
    gate.requirement_count=1U;
    assert(umi_developer_acceptance_gate_evaluate(&gate,&store,&result)==UMI_STATUS_OK);
    assert(!result.passed&&result.blocked_count==1U);
    return 0;
}

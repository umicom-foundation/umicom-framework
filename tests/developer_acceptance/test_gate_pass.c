/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_gate_pass.c
 *
 * PURPOSE:
 *   Verify Framework developer acceptance gate pass behaviour.
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
#include "umicom/developer/acceptance/gate.h"
/*
 * Exercise test developer acceptance gate pass and return a clear result when the
 * behaviour no longer matches its contract.
 */
int test_developer_acceptance_gate_pass(void){UmiDeveloperAcceptanceEvidenceStore s;UmiDeveloperAcceptanceEvidence e={0};UmiDeveloperAcceptanceGate g={0};UmiDeveloperAcceptanceGateResult r;umi_developer_acceptance_evidence_store_init(&s);strcpy(e.capability_id,"cap");e.state=UMI_DEVELOPER_ACCEPTANCE_PASS;assert(umi_developer_acceptance_evidence_upsert(&s,&e)==UMI_STATUS_OK);strcpy(g.gate_id,"g");g.required_capability_ids[0]="cap";g.requirement_count=1;assert(umi_developer_acceptance_gate_evaluate(&g,&s,&r)==UMI_STATUS_OK);assert(r.passed);return 0;}

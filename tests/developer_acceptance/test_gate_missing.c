/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_gate_missing.c
 *
 * PURPOSE:
 *   Verify Framework developer acceptance gate missing behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/acceptance/gate.h"
int test_developer_acceptance_gate_missing(void){UmiDeveloperAcceptanceEvidenceStore s;UmiDeveloperAcceptanceGate g={0};UmiDeveloperAcceptanceGateResult r;umi_developer_acceptance_evidence_store_init(&s);strcpy(g.gate_id,"g");g.required_capability_ids[0]="missing";g.requirement_count=1;assert(umi_developer_acceptance_gate_evaluate(&g,&s,&r)==UMI_STATUS_OK);assert(!r.passed&&r.missing_count==1U);return 0;}

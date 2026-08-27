/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_evidence.c
 *
 * PURPOSE:
 *   Verify Framework developer acceptance evidence behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/acceptance/evidence.h"
int test_developer_acceptance_evidence(void){UmiDeveloperAcceptanceEvidenceStore s;UmiDeveloperAcceptanceEvidence e={0};umi_developer_acceptance_evidence_store_init(&s);strcpy(e.capability_id,"cap");strcpy(e.check_id,"check");e.state=UMI_DEVELOPER_ACCEPTANCE_PASS;assert(umi_developer_acceptance_evidence_upsert(&s,&e)==UMI_STATUS_OK);assert(umi_developer_acceptance_evidence_find(&s,"cap")!=NULL);return 0;}

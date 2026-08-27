/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_evidence_replace.c
 *
 * PURPOSE:
 *   Verify developer acceptance evidence replace behavior.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "umicom/developer/acceptance/evidence.h"
int test_developer_acceptance_evidence_replace(void)
{
    UmiDeveloperAcceptanceEvidenceStore store;
    UmiDeveloperAcceptanceEvidence evidence={0};
    const UmiDeveloperAcceptanceEvidence *found;
    umi_developer_acceptance_evidence_store_init(&store);
    strcpy(evidence.capability_id,"cap");
    evidence.state=UMI_DEVELOPER_ACCEPTANCE_FAIL;
    assert(umi_developer_acceptance_evidence_upsert(&store,&evidence)==UMI_STATUS_OK);
    evidence.state=UMI_DEVELOPER_ACCEPTANCE_PASS;
    assert(umi_developer_acceptance_evidence_upsert(&store,&evidence)==UMI_STATUS_OK);
    assert(store.count==1U);
    found=umi_developer_acceptance_evidence_find(&store,"cap");
    assert(found!=NULL&&found->state==UMI_DEVELOPER_ACCEPTANCE_PASS);
    return 0;
}

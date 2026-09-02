/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_acceptance/test_evidence_replace.c
 *
 * PURPOSE:
 *   Verify developer acceptance evidence replace behavior.
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
#include "umicom/developer/acceptance/evidence.h"
/*
 * Exercise test developer acceptance evidence replace and return a clear result when the
 * behaviour no longer matches its contract.
 */
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

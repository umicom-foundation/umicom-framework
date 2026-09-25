/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_relationship_store.c
 *
 * PURPOSE:
 *   Verify party/account relationship identity and active-account lookup.
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
#include "umicom/financial_platform/relationship_store.h"

int main(void)
{
    UmiFinancialRelationshipStore store;
    UmiFinancialRelationship link = {0};
    umi_financial_relationship_store_init(&store);
    (void)strcpy(link.relationshipId, "rel-1");
    (void)strcpy(link.partyId, "party-1");
    (void)strcpy(link.legalEntityId, "le-1");
    (void)strcpy(link.accountId, "acct-1");
    (void)strcpy(link.role, "owner");
    link.active = 1;
    assert(umi_financial_relationship_store_add(&store, &link) == UMI_STATUS_OK);
    assert(umi_financial_relationship_store_count_for_account(
               &store, "acct-1") == 1U);
    assert(umi_financial_relationship_store_add(&store, &link) ==
           UMI_STATUS_ALREADY_EXISTS);
    return 0;
}

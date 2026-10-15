/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/financial_platform/relationship_store.c
 *
 * PURPOSE:
 *   Implement bounded cross-domain party/account relationship references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include "umicom/financial_platform/relationship_store.h"
#include <string.h>

void umi_financial_relationship_store_init(UmiFinancialRelationshipStore *store)
{
    if (store == NULL) return;
    (void)memset(store, 0, sizeof(*store));
    store->revision = 1U;
}

const UmiFinancialRelationship *umi_financial_relationship_store_find(
    const UmiFinancialRelationshipStore *store,
    const char *relationshipId)
{
    size_t i;
    if (store == NULL || relationshipId == NULL) return NULL;
    for (i = 0U; i < store->count; ++i) {
        if (strcmp(store->relationships[i].relationshipId, relationshipId) == 0)
            return &store->relationships[i];
    }
    return NULL;
}

/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_financial_relationship_store_add(
    UmiFinancialRelationshipStore *store,
    const UmiFinancialRelationship *relationship)
{
    if (store == NULL || relationship == NULL ||
        relationship->relationshipId[0] == '\0' ||
        relationship->partyId[0] == '\0' ||
        relationship->accountId[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_relationship_store_find(
            store, relationship->relationshipId) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (store->count >= UMI_FINANCIAL_RELATIONSHIP_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    store->relationships[store->count++] = *relationship;
    store->revision += 1U;
    return UMI_STATUS_OK;
}
#endif

UmiStatus umi_financial_relationship_store_add(
    UmiFinancialRelationshipStore *store,
    const UmiFinancialRelationship *relationship)
{
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (store != NULL && store->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    if (store == NULL || relationship == NULL ||
        relationship->relationshipId[0] == '\0' ||
        relationship->partyId[0] == '\0' ||
        relationship->accountId[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_financial_relationship_store_find(
            store, relationship->relationshipId) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (store->count >= UMI_FINANCIAL_RELATIONSHIP_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    store->relationships[store->count++] = *relationship;
    store->revision += 1U;
    return UMI_STATUS_OK;
}

size_t umi_financial_relationship_store_count_for_account(
    const UmiFinancialRelationshipStore *store,
    const char *accountId)
{
    size_t i, count = 0U;
    if (store == NULL || accountId == NULL) return 0U;
    for (i = 0U; i < store->count; ++i) {
        if (store->relationships[i].active &&
            strcmp(store->relationships[i].accountId, accountId) == 0) {
            count += 1U;
        }
    }
    return count;
}

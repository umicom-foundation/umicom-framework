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

#include "control_query.h"

/* Retained for review: the former owner trusted public array bounds and inline
 * identities. Framework now admits every active row before lookup or mutation,
 * retains existing lifecycle rules and publishes checked observations atomically. */
#if 0
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

#endif

UmiStatus umi_financial_relationship_validate(const UmiFinancialRelationship *row)
{
    static const UmiSnapshotTextField fields[] = {
        {"relationshipId", offsetof(UmiFinancialRelationship, relationshipId), sizeof(((UmiFinancialRelationship *)0)->relationshipId), 1},
        {"partyId", offsetof(UmiFinancialRelationship, partyId), sizeof(((UmiFinancialRelationship *)0)->partyId), 1},
        {"legalEntityId", offsetof(UmiFinancialRelationship, legalEntityId), sizeof(((UmiFinancialRelationship *)0)->legalEntityId), 0},
        {"accountId", offsetof(UmiFinancialRelationship, accountId), sizeof(((UmiFinancialRelationship *)0)->accountId), 1},
        {"role", offsetof(UmiFinancialRelationship, role), sizeof(((UmiFinancialRelationship *)0)->role), 0}
    };
    if (row == NULL || UmiSnapshotValidateTextFields(row, sizeof(*row), fields,
            sizeof fields / sizeof fields[0], NULL) != UMI_STATUS_OK ||
        (row->active != 0 && row->active != 1)) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_financial_relationship_store_validate(const UmiFinancialRelationshipStore *owner)
{
    if (owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (owner->count > UMI_FINANCIAL_RELATIONSHIP_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (owner->revision == 0U) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialRelationship *row = &owner->relationships[i];
        if (umi_financial_relationship_validate(row) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
        for (size_t j = 0U; j < i; ++j) {
            const UmiFinancialRelationship *previous = &owner->relationships[j];
            if (strcmp(row->relationshipId, previous->relationshipId) == 0) return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}
void umi_financial_relationship_store_init(UmiFinancialRelationshipStore *owner)
{
    if (owner == NULL) return;
    memset(owner, 0, sizeof(*owner));
    owner->revision = 1U;
}
const UmiFinancialRelationship *umi_financial_relationship_store_find(const UmiFinancialRelationshipStore *owner,
    const char *relationshipId)
{
    char relationshipIdKey[64];
    if (FinancialControlQuery(relationshipId, relationshipIdKey, sizeof relationshipIdKey) != UMI_STATUS_OK) return NULL;
    if (umi_financial_relationship_store_validate(owner) != UMI_STATUS_OK) return NULL;
    for (size_t i = 0U; i < owner->count; ++i) {
        const UmiFinancialRelationship *row = &owner->relationships[i];
        if (strcmp(row->relationshipId, relationshipIdKey) == 0) return row;
    }
    return NULL;
}
UmiStatus umi_financial_relationship_store_add(UmiFinancialRelationshipStore *owner, const UmiFinancialRelationship *row)
{
    if (owner != NULL && owner->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiStatus status = umi_financial_relationship_store_validate(owner);
    if (status == UMI_STATUS_OK) status = umi_financial_relationship_validate(row);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < owner->count; ++i) {
        UmiFinancialRelationship *current = &owner->relationships[i];
        if (strcmp(current->relationshipId, row->relationshipId) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (owner->count == UMI_FINANCIAL_RELATIONSHIP_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    owner->relationships[owner->count++] = *row;
    ++owner->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_financial_relationship_store_count_for_account_checked(
    const UmiFinancialRelationshipStore *store, const char *accountId, size_t *outCount)
{
    if (outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    char key[64];
    UmiStatus status = FinancialControlQuery(accountId, key, sizeof key);
    if (status == UMI_STATUS_OK) status = umi_financial_relationship_store_validate(store);
    if (status != UMI_STATUS_OK) return status;
    size_t count = 0U;
    for (size_t i = 0U; i < store->count; ++i)
        if (store->relationships[i].active && strcmp(store->relationships[i].accountId, key) == 0) ++count;
    *outCount = count;
    return UMI_STATUS_OK;
}
size_t umi_financial_relationship_store_count_for_account(
    const UmiFinancialRelationshipStore *store, const char *accountId)
{
    size_t count;
    return umi_financial_relationship_store_count_for_account_checked(store, accountId, &count) == UMI_STATUS_OK ? count : 0U;
}

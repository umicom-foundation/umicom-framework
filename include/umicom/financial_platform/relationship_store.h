/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/financial_platform/relationship_store.h
 *
 * PURPOSE:
 *   Link canonical party, legal-entity and account identifiers without copying
 *   their underlying domain records.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCIAL_PLATFORM_RELATIONSHIP_STORE_H
#define UMICOM_FINANCIAL_PLATFORM_RELATIONSHIP_STORE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FINANCIAL_RELATIONSHIP_CAPACITY 256U

typedef struct UmiFinancialRelationship {
    char relationshipId[64];
    char partyId[64];
    char legalEntityId[64];
    char accountId[64];
    char role[48];
    int active;
} UmiFinancialRelationship;

typedef struct UmiFinancialRelationshipStore {
    UmiFinancialRelationship relationships[UMI_FINANCIAL_RELATIONSHIP_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiFinancialRelationshipStore;

/* Validate complete caller-owned storage before traversal: bounded terminated
 * text, required identities, finite numeric observations, known states/boolean
 * flags and unique keys. Count must fit the array and revision must be nonzero.
 * These references do not replace canonical domain engines or authenticate
 * permissions. Keep inputs stable on one owner thread and outputs disjoint.
 * Borrowed find results last until the next mutation; queries inspect at most
 * 64 readable bytes and must terminate. Unused rows/text tails are ignored. */
UmiStatus umi_financial_relationship_validate(const UmiFinancialRelationship *row);
UmiStatus umi_financial_relationship_store_validate(const UmiFinancialRelationshipStore *owner);
/* legalEntityId and role remain optional; even empty fields must terminate. */
/* Count active relationships for one account, or leave the disjoint output
 * unchanged on refusal. A missing account produces a successful zero. The old
 * scalar query also returns zero on refusal; use this status in reports. */
UmiStatus umi_financial_relationship_store_count_for_account_checked(
    const UmiFinancialRelationshipStore *store, const char *accountId, size_t *outCount);
void umi_financial_relationship_store_init(UmiFinancialRelationshipStore *store);
/** An exhausted owner revision returns UMI_STATUS_CAPACITY_EXCEEDED
 * before this operation changes state or invokes nested work. Initialize the
 * owner before use and serialize mutations through its owning service. */
UmiStatus umi_financial_relationship_store_add(
    UmiFinancialRelationshipStore *store,
    const UmiFinancialRelationship *relationship);
const UmiFinancialRelationship *umi_financial_relationship_store_find(
    const UmiFinancialRelationshipStore *store,
    const char *relationshipId);
size_t umi_financial_relationship_store_count_for_account(
    const UmiFinancialRelationshipStore *store,
    const char *accountId);

#ifdef __cplusplus
}
#endif
#endif

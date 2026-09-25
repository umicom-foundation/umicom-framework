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

void umi_financial_relationship_store_init(UmiFinancialRelationshipStore *store);
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

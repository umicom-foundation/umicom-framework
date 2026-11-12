/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_relationship_store_admission.c
 *
 * PURPOSE:
 *   Verify relationship store bounded storage, mutation and domain-specific observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/financial_platform/service.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

static UmiFinancialRelationship ValidRow(void)
{
    UmiFinancialRelationship row = {"relationship", "party", "", "account", "", 1};
    return row;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiFinancialRelationshipStore owner, before;
    UmiFinancialRelationship row = ValidRow();
    umi_financial_relationship_store_init(&owner);
    REQUIRE(umi_financial_relationship_store_add(&owner, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(owner.count == 1 && owner.revision == 2);
        REQUIRE(umi_financial_relationship_store_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_relationship_store_find(&owner, "relationship") == &owner.relationships[0]);
    } else if (strcmp(test, "fields") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialRelationship, relationshipId), offsetof(UmiFinancialRelationship, partyId), offsetof(UmiFinancialRelationship, legalEntityId), offsetof(UmiFinancialRelationship, accountId), offsetof(UmiFinancialRelationship, role)};
        const size_t lengths[] = {sizeof row.relationshipId, sizeof row.partyId, sizeof row.legalEntityId, sizeof row.accountId, sizeof row.role};
        const int required[] = {1, 1, 0, 1, 0};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) {
            row = ValidRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &owner, sizeof owner);
            REQUIRE(umi_financial_relationship_store_add(&owner, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
            ((char *)&row)[offsets[i]] = '\0';
            REQUIRE(umi_financial_relationship_validate(&row) == (required[i] ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0 || strcmp(test, "stored-field") == 0) {
        if (strcmp(test, "count") == 0) owner.count = SIZE_MAX;
        if (strcmp(test, "duplicate") == 0) { owner.relationships[1] = row; owner.count = 2; }
        if (strcmp(test, "stored-field") == 0) memset(owner.relationships[0].relationshipId, 'x', sizeof row.relationshipId);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_relationship_store_find(&owner, "relationship") == NULL);
        REQUIRE(umi_financial_relationship_store_add(&owner, &row) != UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "revision") == 0) {
        owner.revision = 0; REQUIRE(umi_financial_relationship_store_validate(&owner) == UMI_STATUS_INVALID_STATE);
        owner.revision = UINT64_MAX; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_relationship_store_add(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_FINANCIAL_RELATIONSHIP_CAPACITY; ++i) {
            owner.relationships[i] = row;
            snprintf(owner.relationships[i].relationshipId, 64, "record.%zu", i);
        }
        owner.count = UMI_FINANCIAL_RELATIONSHIP_CAPACITY; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_relationship_store_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_relationship_store_add(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "query") == 0) {
        char key[64]; memset(key, 'x', sizeof key);
        REQUIRE(umi_financial_relationship_store_find(&owner, key) == NULL);
        REQUIRE(umi_financial_relationship_store_find(&owner, "") == NULL);
    } else if (strcmp(test, "flags") == 0) {
        row = ValidRow(); row.active = 2;
        REQUIRE(umi_financial_relationship_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "checked") == 0) {
        size_t count = 77;
        REQUIRE(umi_financial_relationship_store_count_for_account_checked(&owner, "account", &count) == UMI_STATUS_OK && count == 1);
        owner.relationships[0].active = 0;
        REQUIRE(umi_financial_relationship_store_count_for_account_checked(&owner, "account", &count) == UMI_STATUS_OK && count == 0);
        REQUIRE(umi_financial_relationship_store_count_for_account_checked(&owner, "missing", &count) == UMI_STATUS_OK && count == 0);
        count = 77; owner.relationships[0].active = 2;
        REQUIRE(umi_financial_relationship_store_count_for_account_checked(&owner, "account", &count) == UMI_STATUS_INVALID_STATE && count == 77);
        REQUIRE(umi_financial_relationship_store_count_for_account(&owner, "account") == 0);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_relationship_store_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_financial_relationship_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_relationship_store_add(&owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else return 2;
    return 0;
}

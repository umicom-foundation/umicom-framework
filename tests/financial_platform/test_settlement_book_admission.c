/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_settlement_book_admission.c
 *
 * PURPOSE:
 *   Verify settlement book bounded storage, mutation and domain-specific observations.
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

static UmiFinancialSettlementControl ValidRow(void)
{
    UmiFinancialSettlementControl row = {"settlement", "trade", "account", "GBP", -100, 2, 1000U, UMI_FINANCIAL_SETTLEMENT_PENDING, 1U};
    return row;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiFinancialSettlementBook owner, before;
    UmiFinancialSettlementControl row = ValidRow();
    umi_financial_settlement_book_init(&owner);
    REQUIRE(umi_financial_settlement_book_add(&owner, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(owner.count == 1 && owner.revision == 2);
        REQUIRE(umi_financial_settlement_book_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_settlement_book_find(&owner, "settlement") == &owner.records[0]);
    } else if (strcmp(test, "fields") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialSettlementControl, settlementId), offsetof(UmiFinancialSettlementControl, tradeOrPaymentId), offsetof(UmiFinancialSettlementControl, accountId), offsetof(UmiFinancialSettlementControl, currency)};
        const size_t lengths[] = {sizeof row.settlementId, sizeof row.tradeOrPaymentId, sizeof row.accountId, sizeof row.currency};
        const int required[] = {1, 1, 1, 1};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) {
            row = ValidRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &owner, sizeof owner);
            REQUIRE(umi_financial_settlement_book_add(&owner, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
            ((char *)&row)[offsets[i]] = '\0';
            REQUIRE(umi_financial_settlement_control_validate(&row) == (required[i] ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0 || strcmp(test, "stored-field") == 0) {
        if (strcmp(test, "count") == 0) owner.count = SIZE_MAX;
        if (strcmp(test, "duplicate") == 0) { owner.records[1] = row; owner.count = 2; }
        if (strcmp(test, "stored-field") == 0) memset(owner.records[0].settlementId, 'x', sizeof row.settlementId);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_settlement_book_find(&owner, "settlement") == NULL);
        REQUIRE(umi_financial_settlement_book_add(&owner, &row) != UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "revision") == 0) {
        owner.revision = 0; REQUIRE(umi_financial_settlement_book_validate(&owner) == UMI_STATUS_INVALID_STATE);
        owner.revision = UINT64_MAX; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_settlement_book_add(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_FINANCIAL_SETTLEMENT_CAPACITY; ++i) {
            owner.records[i] = row;
            snprintf(owner.records[i].settlementId, 64, "record.%zu", i);
        }
        owner.count = UMI_FINANCIAL_SETTLEMENT_CAPACITY; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_settlement_book_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_settlement_book_add(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "query") == 0) {
        char key[64]; memset(key, 'x', sizeof key);
        REQUIRE(umi_financial_settlement_book_find(&owner, key) == NULL);
        REQUIRE(umi_financial_settlement_book_find(&owner, "") == NULL);
    } else if (strcmp(test, "numbers") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialSettlementControl, cashAmount), offsetof(UmiFinancialSettlementControl, securitiesQuantity)};
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) for (size_t j = 0; j < 3; ++j) {
            row = ValidRow(); memcpy((char *)&row + offsets[i], &bad[j], sizeof(double));
            REQUIRE(umi_financial_settlement_control_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(test, "states") == 0) {
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_settlement_book_transition(&owner, "settlement", (UmiFinancialSettlementState)99, 2) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        row.state = (UmiFinancialSettlementState)99;
        REQUIRE(umi_financial_settlement_control_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "transitions") == 0) {
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_settlement_book_transition(&owner, "settlement", UMI_FINANCIAL_SETTLEMENT_PENDING, 1) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        REQUIRE(umi_financial_settlement_book_transition(&owner, "missing", UMI_FINANCIAL_SETTLEMENT_PENDING, 2) == UMI_STATUS_NOT_FOUND);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        REQUIRE(umi_financial_settlement_book_transition(&owner, "settlement", UMI_FINANCIAL_SETTLEMENT_SETTLED, 2) == UMI_STATUS_OK);
        REQUIRE(owner.records[0].version == 2 && owner.revision == 3);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_settlement_book_transition(&owner, "settlement", UMI_FINANCIAL_SETTLEMENT_PENDING, 3) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "checked") == 0) {
        size_t count = 77;
        REQUIRE(umi_financial_settlement_book_overdue_count_checked(&owner, 1000, &count) == UMI_STATUS_OK && count == 0);
        REQUIRE(umi_financial_settlement_book_overdue_count_checked(&owner, 1001, &count) == UMI_STATUS_OK && count == 1);
        owner.records[0].dueMilliseconds = 0;
        REQUIRE(umi_financial_settlement_book_overdue_count_checked(&owner, UINT64_MAX, &count) == UMI_STATUS_OK && count == 0);
        owner.records[0].dueMilliseconds = 1000; owner.records[0].state = UMI_FINANCIAL_SETTLEMENT_SETTLED;
        REQUIRE(umi_financial_settlement_book_overdue_count_checked(&owner, 1001, &count) == UMI_STATUS_OK && count == 0);
        count = 77; owner.count = SIZE_MAX;
        REQUIRE(umi_financial_settlement_book_overdue_count_checked(&owner, 1001, &count) == UMI_STATUS_CAPACITY_EXCEEDED && count == 77);
        REQUIRE(umi_financial_settlement_book_overdue_count(&owner, 1001) == 0);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_settlement_book_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_financial_settlement_control_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_settlement_book_add(&owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else return 2;
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_position_control_book_admission.c
 *
 * PURPOSE:
 *   Verify position control book bounded storage, mutation and domain-specific observations.
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

static UmiFinancialPositionControl ValidRow(void)
{
    UmiFinancialPositionControl row = {"account", "instrument", "GBP", 2, 90, 100, 5, 1U};
    return row;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiFinancialPositionControlBook owner, before;
    UmiFinancialPositionControl row = ValidRow();
    umi_financial_position_control_book_init(&owner);
    REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(owner.count == 1 && owner.revision == 2);
        REQUIRE(umi_financial_position_control_book_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_position_control_book_find(&owner, "account", "instrument") == &owner.positions[0]);
    } else if (strcmp(test, "fields") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialPositionControl, accountId), offsetof(UmiFinancialPositionControl, instrumentId), offsetof(UmiFinancialPositionControl, currency)};
        const size_t lengths[] = {sizeof row.accountId, sizeof row.instrumentId, sizeof row.currency};
        const int required[] = {1, 1, 1};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) {
            row = ValidRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &owner, sizeof owner);
            REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
            ((char *)&row)[offsets[i]] = '\0';
            REQUIRE(umi_financial_position_control_validate(&row) == (required[i] ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0 || strcmp(test, "stored-field") == 0) {
        if (strcmp(test, "count") == 0) owner.count = SIZE_MAX;
        if (strcmp(test, "duplicate") == 0) { owner.positions[1] = row; owner.count = 2; }
        if (strcmp(test, "stored-field") == 0) memset(owner.positions[0].accountId, 'x', sizeof row.accountId);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_position_control_book_find(&owner, "account", "instrument") == NULL);
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) != UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "revision") == 0) {
        owner.revision = 0; REQUIRE(umi_financial_position_control_book_validate(&owner) == UMI_STATUS_INVALID_STATE);
        owner.revision = UINT64_MAX; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_FINANCIAL_POSITION_CONTROL_CAPACITY; ++i) {
            owner.positions[i] = row;
            snprintf(owner.positions[i].accountId, 64, "record.%zu", i);
        }
        owner.count = UMI_FINANCIAL_POSITION_CONTROL_CAPACITY; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_position_control_book_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        row = owner.positions[owner.count - 1U];
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "query") == 0) {
        char key[64]; memset(key, 'x', sizeof key);
        REQUIRE(umi_financial_position_control_book_find(&owner, key, "instrument") == NULL);
        REQUIRE(umi_financial_position_control_book_find(&owner, "", "instrument") == NULL);
        REQUIRE(umi_financial_position_control_book_find(&owner, "account", key) == NULL);
    } else if (strcmp(test, "numbers") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialPositionControl, quantity), offsetof(UmiFinancialPositionControl, averagePrice), offsetof(UmiFinancialPositionControl, marketPrice), offsetof(UmiFinancialPositionControl, realisedPnl)};
        const double bad[] = {NAN, INFINITY, -INFINITY};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) for (size_t j = 0; j < 3; ++j) {
            row = ValidRow(); memcpy((char *)&row + offsets[i], &bad[j], sizeof(double));
            REQUIRE(umi_financial_position_control_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
        }
    } else if (strcmp(test, "versions") == 0) {
        row.sourceVersion = 0; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        row.sourceVersion = 1; REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "checked") == 0) {
        double value = 77;
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "account", &value) == UMI_STATUS_OK && value == 200);
        strcpy(row.accountId, "other-account"); strcpy(row.currency, "USD");
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "account", &value) == UMI_STATUS_OK && value == 200);
        strcpy(row.accountId, "account"); strcpy(row.instrumentId, "second-instrument");
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "account", &value) == UMI_STATUS_INVALID_STATE && value == 200);
        strcpy(row.currency, "GBP"); row.quantity = -1;
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "account", &value) == UMI_STATUS_OK && value == 100);
        row.quantity = 2; row.marketPrice = DBL_MAX;
        REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "account", &value) == UMI_STATUS_CAPACITY_EXCEEDED && value == 100);
        REQUIRE(umi_financial_position_control_book_market_value(&owner, "account") == 0);
        row.quantity = 1; REQUIRE(umi_financial_position_control_book_upsert(&owner, &row) == UMI_STATUS_OK);
        owner.positions[0].quantity = 1; owner.positions[0].marketPrice = DBL_MAX;
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "account", &value) == UMI_STATUS_CAPACITY_EXCEEDED && value == 100);
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "missing", &value) == UMI_STATUS_OK && value == 0);
        value = 77; owner.positions[1].marketPrice = NAN;
        REQUIRE(umi_financial_position_control_book_market_value_checked(&owner, "account", &value) == UMI_STATUS_INVALID_STATE && value == 77);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_position_control_book_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_financial_position_control_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_position_control_book_upsert(&owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else return 2;
    return 0;
}

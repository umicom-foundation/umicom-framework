/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/financial_platform/test_instrument_link_store_admission.c
 *
 * PURPOSE:
 *   Verify instrument link store bounded storage, mutation and domain-specific observations.
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

static UmiFinancialInstrumentLink ValidRow(void)
{
    UmiFinancialInstrumentLink row = {"instrument", "product", "GBP", "", 1U, 1};
    return row;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *test = argv[1];
    static UmiFinancialInstrumentLinkStore owner, before;
    UmiFinancialInstrumentLink row = ValidRow();
    umi_financial_instrument_link_store_init(&owner);
    REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) == UMI_STATUS_OK);
    if (strcmp(test, "valid") == 0) {
        REQUIRE(owner.count == 1 && owner.revision == 2);
        REQUIRE(umi_financial_instrument_link_store_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_instrument_link_store_find(&owner, "instrument") == &owner.links[0]);
    } else if (strcmp(test, "fields") == 0) {
        const size_t offsets[] = {offsetof(UmiFinancialInstrumentLink, instrumentId), offsetof(UmiFinancialInstrumentLink, productId), offsetof(UmiFinancialInstrumentLink, currency), offsetof(UmiFinancialInstrumentLink, settlementConvention)};
        const size_t lengths[] = {sizeof row.instrumentId, sizeof row.productId, sizeof row.currency, sizeof row.settlementConvention};
        const int required[] = {1, 1, 1, 0};
        for (size_t i = 0; i < sizeof offsets / sizeof offsets[0]; ++i) {
            row = ValidRow(); memset((char *)&row + offsets[i], 'x', lengths[i]);
            memcpy(&before, &owner, sizeof owner);
            REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
            ((char *)&row)[offsets[i]] = '\0';
            REQUIRE(umi_financial_instrument_link_validate(&row) == (required[i] ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
        }
    } else if (strcmp(test, "count") == 0 || strcmp(test, "duplicate") == 0 || strcmp(test, "stored-field") == 0) {
        if (strcmp(test, "count") == 0) owner.count = SIZE_MAX;
        if (strcmp(test, "duplicate") == 0) { owner.links[1] = row; owner.count = 2; }
        if (strcmp(test, "stored-field") == 0) memset(owner.links[0].instrumentId, 'x', sizeof row.instrumentId);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_instrument_link_store_find(&owner, "instrument") == NULL);
        REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) != UMI_STATUS_OK);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "revision") == 0) {
        owner.revision = 0; REQUIRE(umi_financial_instrument_link_store_validate(&owner) == UMI_STATUS_INVALID_STATE);
        owner.revision = UINT64_MAX; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else if (strcmp(test, "capacity") == 0) {
        for (size_t i = 0; i < UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY; ++i) {
            owner.links[i] = row;
            snprintf(owner.links[i].instrumentId, 64, "record.%zu", i);
        }
        owner.count = UMI_FINANCIAL_INSTRUMENT_LINK_CAPACITY; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_instrument_link_store_validate(&owner) == UMI_STATUS_OK);
        REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) == UMI_STATUS_CAPACITY_EXCEEDED);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        row = owner.links[owner.count - 1U];
        REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "query") == 0) {
        char key[64]; memset(key, 'x', sizeof key);
        REQUIRE(umi_financial_instrument_link_store_find(&owner, key) == NULL);
        REQUIRE(umi_financial_instrument_link_store_find(&owner, "") == NULL);
    } else if (strcmp(test, "flags") == 0) {
        row = ValidRow(); row.active = 2;
        REQUIRE(umi_financial_instrument_link_validate(&row) == UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(test, "versions") == 0) {
        row.version = 0; memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) == UMI_STATUS_INVALID_STATE);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
        row.version = 1; REQUIRE(umi_financial_instrument_link_store_upsert(&owner, &row) == UMI_STATUS_OK);
    } else if (strcmp(test, "null") == 0) {
        REQUIRE(umi_financial_instrument_link_store_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(umi_financial_instrument_link_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        memcpy(&before, &owner, sizeof owner);
        REQUIRE(umi_financial_instrument_link_store_upsert(&owner, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        REQUIRE(memcmp(&before, &owner, sizeof owner) == 0);
    } else return 2;
    return 0;
}

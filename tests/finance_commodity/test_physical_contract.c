/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_physical_contract.c
 *
 * PURPOSE:
 *   Implement the test physical contract behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/physical_contract.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/physical_contract.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityPhysicalContractTransferEqual(const UmiCommodityPhysicalContract *a, const UmiCommodityPhysicalContract *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->commodity_id.value, b->commodity_id.value) == 0 &&
        strcmp(a->buyer_party_id.value, b->buyer_party_id.value) == 0 &&
        strcmp(a->seller_party_id.value, b->seller_party_id.value) == 0 &&
        a->quantity.units == b->quantity.units &&
        a->quantity.scale == b->quantity.scale &&
        strcmp(a->quantity.unit_code, b->quantity.unit_code) == 0 &&
        strcmp(a->price_currency.code, b->price_currency.code) == 0 &&
        a->price_minor_units_per_unit == b->price_minor_units_per_unit &&
        a->delivery_start_ms == b->delivery_start_ms &&
        a->delivery_end_ms == b->delivery_end_ms &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityPhysicalContractTransferTails(UmiCommodityPhysicalContract *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->commodity_id.value) + 1U;
        memset(value->commodity_id.value + used, 0xa5, sizeof(value->commodity_id.value) - used);
    }
    {
        size_t used = strlen(value->buyer_party_id.value) + 1U;
        memset(value->buyer_party_id.value + used, 0xa5, sizeof(value->buyer_party_id.value) - used);
    }
    {
        size_t used = strlen(value->seller_party_id.value) + 1U;
        memset(value->seller_party_id.value + used, 0xa5, sizeof(value->seller_party_id.value) - used);
    }
    {
        size_t used = strlen(value->quantity.unit_code) + 1U;
        memset(value->quantity.unit_code + used, 0xa5, sizeof(value->quantity.unit_code) - used);
    }
    {
        size_t used = strlen(value->price_currency.code) + 1U;
        memset(value->price_currency.code + used, 0xa5, sizeof(value->price_currency.code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityPhysicalContractTransferMalformed(const UmiCommodityPhysicalContract *sample)
{
    (void)sample;
    {
        UmiCommodityPhysicalContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_physical_contract_valid(&invalid)) ||
            umi_commodity_physical_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityPhysicalContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.commodity_id.value, 'x', sizeof(invalid.commodity_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_physical_contract_valid(&invalid)) ||
            umi_commodity_physical_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated commodity_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityPhysicalContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.buyer_party_id.value, 'x', sizeof(invalid.buyer_party_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_physical_contract_valid(&invalid)) ||
            umi_commodity_physical_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated buyer_party_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityPhysicalContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.seller_party_id.value, 'x', sizeof(invalid.seller_party_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_physical_contract_valid(&invalid)) ||
            umi_commodity_physical_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated seller_party_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityPhysicalContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.quantity.unit_code, 'x', sizeof(invalid.quantity.unit_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_physical_contract_valid(&invalid)) ||
            umi_commodity_physical_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated quantity.unit_code was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiCommodityPhysicalContract invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.price_currency.code, 'x', sizeof(invalid.price_currency.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_commodity_physical_contract_valid(&invalid)) ||
            umi_commodity_physical_contract_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated price_currency.code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityPhysicalContractTransferCases, UmiCommodityPhysicalContract,
    umi_commodity_physical_contract_archive_encode, umi_commodity_physical_contract_archive_decode,
    UmiCommodityPhysicalContractTransferEqual, UmiCommodityPhysicalContractTransferTails, UmiCommodityPhysicalContractTransferMalformed)

int main(void)
{
    UmiCommodityPhysicalContract value;
    UmiFinancialId buyer = {{"BUYER"}};
    UmiFinancialId seller = {{"SELLER"}};
    UmiCurrency currency = {{'U','S','D','\0'}};
    CHECK(umi_commodity_physical_contract_init(&value, "CTR-1", "CMD-WTI", &buyer, &seller, 1000, 0, "BBL", &currency, 7500, 1000, 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_physical_contract_valid(&value));
    if (UmiCommodityPhysicalContractTransferCases(&value) != 0) return 1;

    return 0;
}

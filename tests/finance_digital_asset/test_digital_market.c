/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_digital_market.c
 *
 * PURPOSE:
 *   Implement the test digital market behavior for
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

#include "umicom/finance/digital_asset/digital_market.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/digital_market.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalMarketTransferEqual(const UmiDigitalMarket *a, const UmiDigitalMarket *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->base_asset_id.value, b->base_asset_id.value) == 0 &&
        strcmp(a->quote_asset_id.value, b->quote_asset_id.value) == 0 &&
        strcmp(a->venue, b->venue) == 0 &&
        a->minimum_quantity_units == b->minimum_quantity_units &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalMarketTransferTails(UmiDigitalMarket *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->base_asset_id.value) + 1U;
        memset(value->base_asset_id.value + used, 0xa5, sizeof(value->base_asset_id.value) - used);
    }
    {
        size_t used = strlen(value->quote_asset_id.value) + 1U;
        memset(value->quote_asset_id.value + used, 0xa5, sizeof(value->quote_asset_id.value) - used);
    }
    {
        size_t used = strlen(value->venue) + 1U;
        memset(value->venue + used, 0xa5, sizeof(value->venue) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalMarketTransferMalformed(const UmiDigitalMarket *sample)
{
    (void)sample;
    {
        UmiDigitalMarket invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_digital_market_valid(&invalid)) ||
            umi_digital_asset_digital_market_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalMarket invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.base_asset_id.value, 'x', sizeof(invalid.base_asset_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_digital_market_valid(&invalid)) ||
            umi_digital_asset_digital_market_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated base_asset_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalMarket invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.quote_asset_id.value, 'x', sizeof(invalid.quote_asset_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_digital_market_valid(&invalid)) ||
            umi_digital_asset_digital_market_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated quote_asset_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalMarket invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.venue, 'x', sizeof(invalid.venue));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_digital_market_valid(&invalid)) ||
            umi_digital_asset_digital_market_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated venue was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalMarketTransferCases, UmiDigitalMarket,
    umi_digital_asset_digital_market_archive_encode, umi_digital_asset_digital_market_archive_decode,
    UmiDigitalMarketTransferEqual, UmiDigitalMarketTransferTails, UmiDigitalMarketTransferMalformed)

int main(void)
{
    UmiDigitalMarket value;
    CHECK(umi_digital_asset_digital_market_init(&value, "BTC-USD", "ASSET-BTC", "ASSET-USD", "UMICOM-X", 1) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_digital_market_valid(&value));
    if (UmiDigitalMarketTransferCases(&value) != 0) return 1;

    return 0;
}

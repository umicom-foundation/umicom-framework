/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_fee_quote.c
 *
 * PURPOSE:
 *   Implement the test fee quote behavior for
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

#include "umicom/finance/digital_asset/fee_quote.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/digital_asset/fee_quote.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiDigitalFeeQuoteTransferEqual(const UmiDigitalFeeQuote *a, const UmiDigitalFeeQuote *b)
{
    return strcmp(a->network_id.value, b->network_id.value) == 0 &&
        a->estimated_fee.units == b->estimated_fee.units &&
        a->estimated_fee.scale == b->estimated_fee.scale &&
        strcmp(a->estimated_fee.asset_symbol, b->estimated_fee.asset_symbol) == 0 &&
        a->expires_time_ms == b->expires_time_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiDigitalFeeQuoteTransferTails(UmiDigitalFeeQuote *value)
{
    (void)value;
    {
        size_t used = strlen(value->network_id.value) + 1U;
        memset(value->network_id.value + used, 0xa5, sizeof(value->network_id.value) - used);
    }
    {
        size_t used = strlen(value->estimated_fee.asset_symbol) + 1U;
        memset(value->estimated_fee.asset_symbol + used, 0xa5, sizeof(value->estimated_fee.asset_symbol) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiDigitalFeeQuoteTransferMalformed(const UmiDigitalFeeQuote *sample)
{
    (void)sample;
    {
        UmiDigitalFeeQuote invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.network_id.value, 'x', sizeof(invalid.network_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_fee_quote_valid(&invalid)) ||
            umi_digital_asset_fee_quote_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated network_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiDigitalFeeQuote invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.estimated_fee.asset_symbol, 'x', sizeof(invalid.estimated_fee.asset_symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_digital_asset_fee_quote_valid(&invalid)) ||
            umi_digital_asset_fee_quote_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated estimated_fee.asset_symbol was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiDigitalFeeQuoteTransferCases, UmiDigitalFeeQuote,
    umi_digital_asset_fee_quote_archive_encode, umi_digital_asset_fee_quote_archive_decode,
    UmiDigitalFeeQuoteTransferEqual, UmiDigitalFeeQuoteTransferTails, UmiDigitalFeeQuoteTransferMalformed)

int main(void)
{
    UmiDigitalFeeQuote value;
    CHECK(umi_digital_asset_fee_quote_init(&value, "BTC", 1200, 8, "BTC", 5000) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_fee_quote_valid(&value));
    if (UmiDigitalFeeQuoteTransferCases(&value) != 0) return 1;

    return 0;
}

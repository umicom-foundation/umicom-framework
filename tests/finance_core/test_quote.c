/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_quote.c
 *
 * PURPOSE:
 *   Exercise the quote financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/quote.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Market-data-key quote transfer is a Finance contract. These helpers were
 * previously registered against the incompatible Trading quote fixture.
 * Keep all field/tail/refusal checks but validate with the Finance owner. */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/core/quote.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiQuoteTransferEqual(const UmiQuote *a, const UmiQuote *b)
{
    return strcmp(a->key.key_id.value, b->key.key_id.value) == 0 &&
        strcmp(a->key.name, b->key.name) == 0 &&
        strcmp(a->key.code, b->key.code) == 0 &&
        a->key.state == b->key.state &&
        a->key.active == b->key.active &&
        a->side == b->side &&
        a->price.value == b->price.value &&
        a->price.scale == b->price.scale &&
        a->timestamp == b->timestamp;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiQuoteTransferTails(UmiQuote *value)
{
    (void)value;
    {
        size_t used = strlen(value->key.key_id.value) + 1U;
        memset(value->key.key_id.value + used, 0xa5, sizeof(value->key.key_id.value) - used);
    }
    {
        size_t used = strlen(value->key.name) + 1U;
        memset(value->key.name + used, 0xa5, sizeof(value->key.name) - used);
    }
    {
        size_t used = strlen(value->key.code) + 1U;
        memset(value->key.code + used, 0xa5, sizeof(value->key.code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiQuoteTransferMalformed(const UmiQuote *sample)
{
    (void)sample;
    {
        UmiQuote invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key.key_id.value, 'x', sizeof(invalid.key.key_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_quote_is_valid(&invalid)) ||
            umi_quote_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key.key_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiQuote invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key.name, 'x', sizeof(invalid.key.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_quote_is_valid(&invalid)) ||
            umi_quote_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key.name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiQuote invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.key.code, 'x', sizeof(invalid.key.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_quote_is_valid(&invalid)) ||
            umi_quote_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated key.code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiQuoteTransferCases, UmiQuote,
    umi_quote_archive_encode, umi_quote_archive_decode,
    UmiQuoteTransferEqual, UmiQuoteTransferTails, UmiQuoteTransferMalformed)

int main(void)
{
    UmiMarketDataKey k; UmiFinancialPrice p; UmiQuote q; CHECK(umi_market_data_key_init(&k,"K","Key","SRC",1U)==UMI_STATUS_OK); CHECK(umi_price_init(&p,1.0,2U)==UMI_STATUS_OK); CHECK(umi_quote_init(&q,&k,UMI_QUOTE_MID,p,1)==UMI_STATUS_OK);
    /* Verify the baseline Finance quote and every preserved archive boundary case. */
    CHECK(umi_quote_is_valid(&q));
    CHECK(UmiQuoteTransferCases(&q) == 0);
    /* Distinct non-default fields prove that every Finance field, rather than
     * Trading bid/ask bytes or object padding, survives the public codec. */
    q.key.state = UINT32_C(123);
    q.key.active = false;
    q.side = UMI_QUOTE_LAST;
    q.price.value = 125.75;
    q.price.scale = 2U;
    q.timestamp = INT64_C(2147483648);
    CHECK(UmiQuoteTransferCases(&q) == 0);
    return 0;
}

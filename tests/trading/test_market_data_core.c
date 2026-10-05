/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_market_data_core.c
 *
 * PURPOSE:
 *   Validate market data core behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "test_trading_common.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBarTransferEqual(const UmiBar *a, const UmiBar *b)
{
    return strcmp(a->instrument.instrument_id.value, b->instrument.instrument_id.value) == 0 &&
        strcmp(a->instrument.symbol, b->instrument.symbol) == 0 &&
        strcmp(a->instrument.venue, b->instrument.venue) == 0 &&
        strcmp(a->instrument.currency.code, b->instrument.currency.code) == 0 &&
        a->instrument.multiplier == b->instrument.multiplier &&
        a->instrument.expiry_yyyymmdd == b->instrument.expiry_yyyymmdd &&
        a->open == b->open &&
        a->high == b->high &&
        a->low == b->low &&
        a->close == b->close &&
        a->volume == b->volume &&
        a->start_time_ms == b->start_time_ms &&
        a->end_time_ms == b->end_time_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBarTransferTails(UmiBar *value)
{
    (void)value;
    {
        size_t used = strlen(value->instrument.instrument_id.value) + 1U;
        memset(value->instrument.instrument_id.value + used, 0xa5, sizeof(value->instrument.instrument_id.value) - used);
    }
    {
        size_t used = strlen(value->instrument.symbol) + 1U;
        memset(value->instrument.symbol + used, 0xa5, sizeof(value->instrument.symbol) - used);
    }
    {
        size_t used = strlen(value->instrument.venue) + 1U;
        memset(value->instrument.venue + used, 0xa5, sizeof(value->instrument.venue) - used);
    }
    {
        size_t used = strlen(value->instrument.currency.code) + 1U;
        memset(value->instrument.currency.code + used, 0xa5, sizeof(value->instrument.currency.code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBarTransferMalformed(const UmiBar *sample)
{
    (void)sample;
    {
        UmiBar invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.instrument_id.value, 'x', sizeof(invalid.instrument.instrument_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bar_valid(&invalid)) ||
            umi_bar_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.instrument_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBar invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.symbol, 'x', sizeof(invalid.instrument.symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bar_valid(&invalid)) ||
            umi_bar_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.symbol was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBar invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.venue, 'x', sizeof(invalid.instrument.venue));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bar_valid(&invalid)) ||
            umi_bar_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.venue was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBar invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.currency.code, 'x', sizeof(invalid.instrument.currency.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_bar_valid(&invalid)) ||
            umi_bar_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.currency.code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBarTransferCases, UmiBar,
    umi_bar_archive_encode, umi_bar_archive_decode,
    UmiBarTransferEqual, UmiBarTransferTails, UmiBarTransferMalformed)

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
        if (!(!umi_quote_valid(&invalid)) ||
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
        if (!(!umi_quote_valid(&invalid)) ||
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
        if (!(!umi_quote_valid(&invalid)) ||
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

int main(void){
    UmiQuote q={0};q.instrument=test_instrument();q.bid=25000.0;q.ask=25002.0;q.bid_size=4.0;q.ask_size=2.0;q.event_time_ms=1000;
    assert(umi_quote_valid(&q));
    if (UmiQuoteTransferCases(&q) != 0) return 1;
assert(umi_quote_mid(&q)==25001.0);assert(umi_quote_spread(&q)==2.0);
    UmiBar b={0};b.instrument=q.instrument;b.open=24990;b.high=25010;b.low=24980;b.close=25000;b.volume=100;b.start_time_ms=0;b.end_time_ms=1000;
    assert(umi_bar_valid(&b));
    if (UmiBarTransferCases(&b) != 0) return 1;
assert(umi_bar_range(&b)==30.0);assert(umi_market_data_snapshot_aligned(&q,&b,0));
    assert(umi_market_data_quality_score(&q,1100,1000)>0.8);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_market_data_event.c
 *
 * PURPOSE:
 *   Exercise normalise venue market-data sequence, instrument identity and event time.
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
#include "umicom/trading/core/market_data_event.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/market_data_event.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingMarketDataEventTransferEqual(const UmiTradingMarketDataEvent *a, const UmiTradingMarketDataEvent *b)
{
    return strcmp(a->instrument_id.value, b->instrument_id.value) == 0 &&
        strcmp(a->venue_id.value, b->venue_id.value) == 0 &&
        a->sequence == b->sequence &&
        a->event_time_ms == b->event_time_ms;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingMarketDataEventTransferTails(UmiTradingMarketDataEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->instrument_id.value) + 1U;
        memset(value->instrument_id.value + used, 0xa5, sizeof(value->instrument_id.value) - used);
    }
    {
        size_t used = strlen(value->venue_id.value) + 1U;
        memset(value->venue_id.value + used, 0xa5, sizeof(value->venue_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingMarketDataEventTransferMalformed(const UmiTradingMarketDataEvent *sample)
{
    (void)sample;
    {
        UmiTradingMarketDataEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument_id.value, 'x', sizeof(invalid.instrument_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_market_data_event_valid(&invalid)) ||
            umi_trading_market_data_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradingMarketDataEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.venue_id.value, 'x', sizeof(invalid.venue_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_market_data_event_valid(&invalid)) ||
            umi_trading_market_data_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated venue_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingMarketDataEventTransferCases, UmiTradingMarketDataEvent,
    umi_trading_market_data_event_archive_encode, umi_trading_market_data_event_archive_decode,
    UmiTradingMarketDataEventTransferEqual, UmiTradingMarketDataEventTransferTails, UmiTradingMarketDataEventTransferMalformed)

int main(void) {
    UmiFinancialId iid,vid;
    umi_trading_core_id_assign(&iid,"i");
    umi_trading_core_id_assign(&vid,"v");
     UmiTradingMarketDataEvent v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_market_data_event_init(&v,&iid,&vid,1U,1000)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_market_data_event_valid(&v)) return 2;
    if (UmiTradingMarketDataEventTransferCases(&v) != 0) return 1;

     return 0;
}

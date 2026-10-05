/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_session.c
 *
 * PURPOSE:
 *   Exercise model a bounded venue trading session and its current phase.
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
#include "umicom/trading/core/venue_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/venue_session.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingVenueSessionTransferEqual(const UmiTradingVenueSession *a, const UmiTradingVenueSession *b)
{
    return strcmp(a->venue_id.value, b->venue_id.value) == 0 &&
        a->open_time_ms == b->open_time_ms &&
        a->close_time_ms == b->close_time_ms &&
        a->phase == b->phase;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingVenueSessionTransferTails(UmiTradingVenueSession *value)
{
    (void)value;
    {
        size_t used = strlen(value->venue_id.value) + 1U;
        memset(value->venue_id.value + used, 0xa5, sizeof(value->venue_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingVenueSessionTransferMalformed(const UmiTradingVenueSession *sample)
{
    (void)sample;
    {
        UmiTradingVenueSession invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.venue_id.value, 'x', sizeof(invalid.venue_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_venue_session_valid(&invalid)) ||
            umi_trading_venue_session_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated venue_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingVenueSessionTransferCases, UmiTradingVenueSession,
    umi_trading_venue_session_archive_encode, umi_trading_venue_session_archive_decode,
    UmiTradingVenueSessionTransferEqual, UmiTradingVenueSessionTransferTails, UmiTradingVenueSessionTransferMalformed)

int main(void) {
    UmiFinancialId id;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_id_assign(&id,"v1")!=UMI_STATUS_OK) return 9;
     UmiTradingVenueSession v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_venue_session_init(&v,&id,1000,2000,UMI_TRADING_CORE_PHASE_CONTINUOUS)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_venue_session_valid(&v)) return 2;
    if (UmiTradingVenueSessionTransferCases(&v) != 0) return 1;

     return 0;
}

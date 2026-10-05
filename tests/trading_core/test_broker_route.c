/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_broker_route.c
 *
 * PURPOSE:
 *   Exercise describe a candidate broker/venue route with cost and latency scores.
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
#include "umicom/trading/core/broker_route.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/broker_route.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingBrokerRouteTransferEqual(const UmiTradingBrokerRoute *a, const UmiTradingBrokerRoute *b)
{
    return strcmp(a->route_id.value, b->route_id.value) == 0 &&
        strcmp(a->venue_id.value, b->venue_id.value) == 0 &&
        a->cost_bps == b->cost_bps &&
        a->latency_score == b->latency_score &&
        a->enabled == b->enabled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingBrokerRouteTransferTails(UmiTradingBrokerRoute *value)
{
    (void)value;
    {
        size_t used = strlen(value->route_id.value) + 1U;
        memset(value->route_id.value + used, 0xa5, sizeof(value->route_id.value) - used);
    }
    {
        size_t used = strlen(value->venue_id.value) + 1U;
        memset(value->venue_id.value + used, 0xa5, sizeof(value->venue_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingBrokerRouteTransferMalformed(const UmiTradingBrokerRoute *sample)
{
    (void)sample;
    {
        UmiTradingBrokerRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.route_id.value, 'x', sizeof(invalid.route_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_broker_route_valid(&invalid)) ||
            umi_trading_broker_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated route_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTradingBrokerRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.venue_id.value, 'x', sizeof(invalid.venue_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_broker_route_valid(&invalid)) ||
            umi_trading_broker_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated venue_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingBrokerRouteTransferCases, UmiTradingBrokerRoute,
    umi_trading_broker_route_archive_encode, umi_trading_broker_route_archive_decode,
    UmiTradingBrokerRouteTransferEqual, UmiTradingBrokerRouteTransferTails, UmiTradingBrokerRouteTransferMalformed)

int main(void) {
    UmiFinancialId rid,vid;
    umi_trading_core_id_assign(&rid,"r");
    umi_trading_core_id_assign(&vid,"v");
     UmiTradingBrokerRoute v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_broker_route_init(&v,&rid,&vid,5U,10U,true)!=UMI_STATUS_OK) return 1;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(!umi_trading_broker_route_valid(&v)) return 2;
    if (UmiTradingBrokerRouteTransferCases(&v) != 0) return 1;

     return 0;
}

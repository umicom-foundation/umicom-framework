/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_event.c
 *
 * PURPOSE:
 *   Exercise capture sequence-ordered evidence for an order lifecycle transition.
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
#include "umicom/trading/core/order_event.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/order_event.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingOrderEventTransferEqual(const UmiTradingOrderEvent *a, const UmiTradingOrderEvent *b)
{
    return strcmp(a->client_order_id.value, b->client_order_id.value) == 0 &&
        a->sequence == b->sequence &&
        a->event_time_ms == b->event_time_ms &&
        a->state == b->state;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingOrderEventTransferTails(UmiTradingOrderEvent *value)
{
    (void)value;
    {
        size_t used = strlen(value->client_order_id.value) + 1U;
        memset(value->client_order_id.value + used, 0xa5, sizeof(value->client_order_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingOrderEventTransferMalformed(const UmiTradingOrderEvent *sample)
{
    (void)sample;
    {
        UmiTradingOrderEvent invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.client_order_id.value, 'x', sizeof(invalid.client_order_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_trading_order_event_valid(&invalid)) ||
            umi_trading_order_event_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated client_order_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingOrderEventTransferCases, UmiTradingOrderEvent,
    umi_trading_order_event_archive_encode, umi_trading_order_event_archive_decode,
    UmiTradingOrderEventTransferEqual, UmiTradingOrderEventTransferTails, UmiTradingOrderEventTransferMalformed)

int main(void) {
    UmiFinancialId id;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_core_id_assign(&id,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingOrderEvent v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_order_event_init(&v,&id,1U,1000,UMI_TRADING_CORE_ORDER_OPEN)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_order_event_valid(&v)) return 2;
    if (UmiTradingOrderEventTransferCases(&v) != 0) return 1;

     return 0;
}

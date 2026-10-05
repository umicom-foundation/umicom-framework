/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_order_lifecycle.c
 *
 * PURPOSE:
 *   Validate order lifecycle behaviour in the trading foundation.
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
static int UmiOrderRequestTransferEqual(const UmiOrderRequest *a, const UmiOrderRequest *b)
{
    return strcmp(a->client_order_id.value, b->client_order_id.value) == 0 &&
        strcmp(a->account_id.value, b->account_id.value) == 0 &&
        strcmp(a->instrument.instrument_id.value, b->instrument.instrument_id.value) == 0 &&
        strcmp(a->instrument.symbol, b->instrument.symbol) == 0 &&
        strcmp(a->instrument.venue, b->instrument.venue) == 0 &&
        strcmp(a->instrument.currency.code, b->instrument.currency.code) == 0 &&
        a->instrument.multiplier == b->instrument.multiplier &&
        a->instrument.expiry_yyyymmdd == b->instrument.expiry_yyyymmdd &&
        a->side == b->side &&
        a->type == b->type &&
        a->tif == b->tif &&
        a->quantity == b->quantity &&
        a->limit_price == b->limit_price &&
        a->stop_price == b->stop_price &&
        a->environment == b->environment;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiOrderRequestTransferTails(UmiOrderRequest *value)
{
    (void)value;
    {
        size_t used = strlen(value->client_order_id.value) + 1U;
        memset(value->client_order_id.value + used, 0xa5, sizeof(value->client_order_id.value) - used);
    }
    {
        size_t used = strlen(value->account_id.value) + 1U;
        memset(value->account_id.value + used, 0xa5, sizeof(value->account_id.value) - used);
    }
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
static int UmiOrderRequestTransferMalformed(const UmiOrderRequest *sample)
{
    (void)sample;
    {
        UmiOrderRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.client_order_id.value, 'x', sizeof(invalid.client_order_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_order_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_order_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated client_order_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiOrderRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.account_id.value, 'x', sizeof(invalid.account_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_order_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_order_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated account_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiOrderRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.instrument_id.value, 'x', sizeof(invalid.instrument.instrument_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_order_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_order_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.instrument_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiOrderRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.symbol, 'x', sizeof(invalid.instrument.symbol));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_order_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_order_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.symbol was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiOrderRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.venue, 'x', sizeof(invalid.instrument.venue));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_order_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_order_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.venue was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiOrderRequest invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument.currency.code, 'x', sizeof(invalid.instrument.currency.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_order_request_validate(&invalid) != UMI_STATUS_OK) ||
            umi_order_request_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument.currency.code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiOrderRequestTransferCases, UmiOrderRequest,
    umi_order_request_archive_encode, umi_order_request_archive_decode,
    UmiOrderRequestTransferEqual, UmiOrderRequestTransferTails, UmiOrderRequestTransferMalformed)

int main(void){
    UmiOrderRequest r=test_order_request();assert(umi_order_request_validate(&r)==UMI_STATUS_OK);
    if (UmiOrderRequestTransferCases(&r) != 0) return 1;

    assert(umi_order_transition_allowed(UMI_ORDER_NEW,UMI_ORDER_VALIDATED));
    assert(umi_order_transition_allowed(UMI_ORDER_ACCEPTED,UMI_ORDER_PARTIALLY_FILLED));
    assert(!umi_order_transition_allowed(UMI_ORDER_FILLED,UMI_ORDER_ACCEPTED));
    assert(umi_order_validate_for_market(&r,UMI_MARKET_OPEN)==UMI_STATUS_OK);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_delivery_window.c
 *
 * PURPOSE:
 *   Implement the test delivery window behavior for
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

#include "umicom/finance/commodity/delivery_window.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/delivery_window.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommodityDeliveryWindowTransferEqual(const UmiCommodityDeliveryWindow *a, const UmiCommodityDeliveryWindow *b)
{
    return a->start_time_ms == b->start_time_ms &&
        a->end_time_ms == b->end_time_ms &&
        a->inclusive_end == b->inclusive_end;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommodityDeliveryWindowTransferTails(UmiCommodityDeliveryWindow *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommodityDeliveryWindowTransferMalformed(const UmiCommodityDeliveryWindow *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommodityDeliveryWindowTransferCases, UmiCommodityDeliveryWindow,
    umi_commodity_delivery_window_archive_encode, umi_commodity_delivery_window_archive_decode,
    UmiCommodityDeliveryWindowTransferEqual, UmiCommodityDeliveryWindowTransferTails, UmiCommodityDeliveryWindowTransferMalformed)

int main(void)
{
    UmiCommodityDeliveryWindow value;
    CHECK(umi_commodity_delivery_window_init(&value, 1000, 2000, true) == UMI_STATUS_OK);
    CHECK(umi_commodity_delivery_window_valid(&value));
    if (UmiCommodityDeliveryWindowTransferCases(&value) != 0) return 1;

    return 0;
}

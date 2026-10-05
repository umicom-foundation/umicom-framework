/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_route.c
 *
 * PURPOSE:
 *   Exercise payment route validation and calculations.
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
#include "umicom/finance/payments/payment_route.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_route.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentRouteTransferEqual(const UmiPaymentsPaymentRoute *a, const UmiPaymentsPaymentRoute *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        a->rail_kind == b->rail_kind &&
        a->priority == b->priority &&
        a->available == b->available;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentRouteTransferTails(UmiPaymentsPaymentRoute *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentRouteTransferMalformed(const UmiPaymentsPaymentRoute *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentRoute invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_route_valid(&invalid)) ||
            umi_payments_payment_route_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentRouteTransferCases, UmiPaymentsPaymentRoute,
    umi_payments_payment_route_archive_encode, umi_payments_payment_route_archive_decode,
    UmiPaymentsPaymentRouteTransferEqual, UmiPaymentsPaymentRouteTransferTails, UmiPaymentsPaymentRouteTransferMalformed)

int main(void) {
    UmiPaymentsPaymentRoute v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_route_init(&v, "route-1", UMI_PAYMENTS_RAIL_HIGH_VALUE, 1U, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_route_valid(&v)) return 2;
    if (UmiPaymentsPaymentRouteTransferCases(&v) != 0) return 1;

    return 0;
}

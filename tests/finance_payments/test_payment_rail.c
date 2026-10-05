/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_rail.c
 *
 * PURPOSE:
 *   Exercise payment rail validation and calculations.
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
#include "umicom/finance/payments/payment_rail.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_rail.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentRailTransferEqual(const UmiPaymentsPaymentRail *a, const UmiPaymentsPaymentRail *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        a->kind == b->kind &&
        strcmp(a->name, b->name) == 0 &&
        a->maximum_minor == b->maximum_minor &&
        a->supports_instant == b->supports_instant;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentRailTransferTails(UmiPaymentsPaymentRail *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentRailTransferMalformed(const UmiPaymentsPaymentRail *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentRail invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_rail_valid(&invalid)) ||
            umi_payments_payment_rail_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentRail invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_rail_valid(&invalid)) ||
            umi_payments_payment_rail_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentRailTransferCases, UmiPaymentsPaymentRail,
    umi_payments_payment_rail_archive_encode, umi_payments_payment_rail_archive_decode,
    UmiPaymentsPaymentRailTransferEqual, UmiPaymentsPaymentRailTransferTails, UmiPaymentsPaymentRailTransferMalformed)

int main(void) {
    UmiPaymentsPaymentRail v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_rail_init(&v, "rail-1", UMI_PAYMENTS_RAIL_INSTANT, "Instant Rail", 1000000, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_rail_valid(&v)) return 2;
    if (UmiPaymentsPaymentRailTransferCases(&v) != 0) return 1;

    return 0;
}

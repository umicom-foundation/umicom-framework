/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_clearing.c
 *
 * PURPOSE:
 *   Exercise payment clearing validation and calculations.
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
#include "umicom/finance/payments/payment_clearing.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_clearing.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentClearingTransferEqual(const UmiPaymentsPaymentClearing *a, const UmiPaymentsPaymentClearing *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        a->gross_minor == b->gross_minor &&
        a->net_minor == b->net_minor &&
        a->participant_count == b->participant_count &&
        a->complete == b->complete;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentClearingTransferTails(UmiPaymentsPaymentClearing *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentClearingTransferMalformed(const UmiPaymentsPaymentClearing *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentClearing invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_clearing_valid(&invalid)) ||
            umi_payments_payment_clearing_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentClearingTransferCases, UmiPaymentsPaymentClearing,
    umi_payments_payment_clearing_archive_encode, umi_payments_payment_clearing_archive_decode,
    UmiPaymentsPaymentClearingTransferEqual, UmiPaymentsPaymentClearingTransferTails, UmiPaymentsPaymentClearingTransferMalformed)

int main(void) {
    UmiPaymentsPaymentClearing v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_clearing_init(&v, "clear-1", 10000, -2000, 4U, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_clearing_valid(&v)) return 2;
    if (UmiPaymentsPaymentClearingTransferCases(&v) != 0) return 1;

    return 0;
}

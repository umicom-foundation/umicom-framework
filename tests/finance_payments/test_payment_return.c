/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_return.c
 *
 * PURPOSE:
 *   Exercise payment return validation and calculations.
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
#include "umicom/finance/payments/payment_return.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_return.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentReturnTransferEqual(const UmiPaymentsPaymentReturn *a, const UmiPaymentsPaymentReturn *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->original_payment_id.value, b->original_payment_id.value) == 0 &&
        strcmp(a->reason_code, b->reason_code) == 0 &&
        a->amount_minor == b->amount_minor;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentReturnTransferTails(UmiPaymentsPaymentReturn *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->original_payment_id.value) + 1U;
        memset(value->original_payment_id.value + used, 0xa5, sizeof(value->original_payment_id.value) - used);
    }
    {
        size_t used = strlen(value->reason_code) + 1U;
        memset(value->reason_code + used, 0xa5, sizeof(value->reason_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentReturnTransferMalformed(const UmiPaymentsPaymentReturn *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentReturn invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_return_valid(&invalid)) ||
            umi_payments_payment_return_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentReturn invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.original_payment_id.value, 'x', sizeof(invalid.original_payment_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_return_valid(&invalid)) ||
            umi_payments_payment_return_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated original_payment_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentReturn invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.reason_code, 'x', sizeof(invalid.reason_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_return_valid(&invalid)) ||
            umi_payments_payment_return_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated reason_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentReturnTransferCases, UmiPaymentsPaymentReturn,
    umi_payments_payment_return_archive_encode, umi_payments_payment_return_archive_decode,
    UmiPaymentsPaymentReturnTransferEqual, UmiPaymentsPaymentReturnTransferTails, UmiPaymentsPaymentReturnTransferMalformed)

int main(void) {
    UmiPaymentsPaymentReturn v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_return_init(&v, "return-1", "pay-1", "AC04", 1500)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_return_valid(&v)) return 2;
    if (UmiPaymentsPaymentReturnTransferCases(&v) != 0) return 1;

    return 0;
}

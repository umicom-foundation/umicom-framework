/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_message.c
 *
 * PURPOSE:
 *   Exercise payment message validation and calculations.
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
#include "umicom/finance/payments/payment_message.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_message.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentMessageTransferEqual(const UmiPaymentsPaymentMessage *a, const UmiPaymentsPaymentMessage *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->payment_id.value, b->payment_id.value) == 0 &&
        strcmp(a->message_type, b->message_type) == 0 &&
        a->direction == b->direction &&
        a->sequence == b->sequence;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentMessageTransferTails(UmiPaymentsPaymentMessage *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->payment_id.value) + 1U;
        memset(value->payment_id.value + used, 0xa5, sizeof(value->payment_id.value) - used);
    }
    {
        size_t used = strlen(value->message_type) + 1U;
        memset(value->message_type + used, 0xa5, sizeof(value->message_type) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentMessageTransferMalformed(const UmiPaymentsPaymentMessage *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_message_valid(&invalid)) ||
            umi_payments_payment_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.payment_id.value, 'x', sizeof(invalid.payment_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_message_valid(&invalid)) ||
            umi_payments_payment_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated payment_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message_type, 'x', sizeof(invalid.message_type));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_message_valid(&invalid)) ||
            umi_payments_payment_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message_type was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentMessageTransferCases, UmiPaymentsPaymentMessage,
    umi_payments_payment_message_archive_encode, umi_payments_payment_message_archive_decode,
    UmiPaymentsPaymentMessageTransferEqual, UmiPaymentsPaymentMessageTransferTails, UmiPaymentsPaymentMessageTransferMalformed)

int main(void) {
    UmiPaymentsPaymentMessage v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_message_init(&v, "msg-1", "pay-1", "PAYMENT", UMI_PAYMENTS_MESSAGE_OUTBOUND, 1U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_message_valid(&v)) return 2;
    if (UmiPaymentsPaymentMessageTransferCases(&v) != 0) return 1;

    return 0;
}

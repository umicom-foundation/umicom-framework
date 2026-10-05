/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_iso_message.c
 *
 * PURPOSE:
 *   Exercise iso message validation and calculations.
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
#include "umicom/finance/payments/iso_message.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/iso_message.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsIsoMessageTransferEqual(const UmiPaymentsIsoMessage *a, const UmiPaymentsIsoMessage *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->payment_id.value, b->payment_id.value) == 0 &&
        strcmp(a->message_family, b->message_family) == 0 &&
        strcmp(a->end_to_end_id, b->end_to_end_id) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsIsoMessageTransferTails(UmiPaymentsIsoMessage *value)
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
        size_t used = strlen(value->message_family) + 1U;
        memset(value->message_family + used, 0xa5, sizeof(value->message_family) - used);
    }
    {
        size_t used = strlen(value->end_to_end_id) + 1U;
        memset(value->end_to_end_id + used, 0xa5, sizeof(value->end_to_end_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsIsoMessageTransferMalformed(const UmiPaymentsIsoMessage *sample)
{
    (void)sample;
    {
        UmiPaymentsIsoMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_iso_message_valid(&invalid)) ||
            umi_payments_iso_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsIsoMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.payment_id.value, 'x', sizeof(invalid.payment_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_iso_message_valid(&invalid)) ||
            umi_payments_iso_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated payment_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsIsoMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.message_family, 'x', sizeof(invalid.message_family));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_iso_message_valid(&invalid)) ||
            umi_payments_iso_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated message_family was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsIsoMessage invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.end_to_end_id, 'x', sizeof(invalid.end_to_end_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_iso_message_valid(&invalid)) ||
            umi_payments_iso_message_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated end_to_end_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsIsoMessageTransferCases, UmiPaymentsIsoMessage,
    umi_payments_iso_message_archive_encode, umi_payments_iso_message_archive_decode,
    UmiPaymentsIsoMessageTransferEqual, UmiPaymentsIsoMessageTransferTails, UmiPaymentsIsoMessageTransferMalformed)

int main(void) {
    UmiPaymentsIsoMessage v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_iso_message_init(&v, "iso-1", "pay-1", "pacs.008", "E2E-1")!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_iso_message_valid(&v)) return 2;
    if (UmiPaymentsIsoMessageTransferCases(&v) != 0) return 1;

    return 0;
}

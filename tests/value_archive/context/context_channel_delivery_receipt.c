/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/value_archive/context/context_channel_delivery_receipt.c
 * PURPOSE: Verify portable context state before a host reviews or applies it.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/context_channel/delivery_receipt.h"

/* This is a passive saved value. Filling its identity and descriptive fields
 * does not create a provider, grant a permission or open a panel. */
#include "../transfer_cases.h"

#include "umicom/context_channel/delivery_receipt.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiContextDeliveryReceiptTransferEqual(const UmiContextDeliveryReceipt *a, const UmiContextDeliveryReceipt *b)
{
    return a->structure_size == b->structure_size &&
        strcmp(a->receipt_id, b->receipt_id) == 0 &&
        strcmp(a->context_id, b->context_id) == 0 &&
        strcmp(a->subscription_id, b->subscription_id) == 0 &&
        strcmp(a->application_id, b->application_id) == 0 &&
        strcmp(a->panel_id, b->panel_id) == 0 &&
        a->state == b->state &&
        a->status == b->status &&
        a->delivered_at_ms == b->delivered_at_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiContextDeliveryReceiptTransferTails(UmiContextDeliveryReceipt *value)
{
    (void)value;
    {
        size_t used = strlen(value->receipt_id) + 1U;
        memset(value->receipt_id + used, 0xa5, sizeof(value->receipt_id) - used);
    }
    {
        size_t used = strlen(value->context_id) + 1U;
        memset(value->context_id + used, 0xa5, sizeof(value->context_id) - used);
    }
    {
        size_t used = strlen(value->subscription_id) + 1U;
        memset(value->subscription_id + used, 0xa5, sizeof(value->subscription_id) - used);
    }
    {
        size_t used = strlen(value->application_id) + 1U;
        memset(value->application_id + used, 0xa5, sizeof(value->application_id) - used);
    }
    {
        size_t used = strlen(value->panel_id) + 1U;
        memset(value->panel_id + used, 0xa5, sizeof(value->panel_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiContextDeliveryReceiptTransferMalformed(const UmiContextDeliveryReceipt *sample)
{
    (void)sample;
    {
        UmiContextDeliveryReceipt invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.receipt_id, 'x', sizeof(invalid.receipt_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_delivery_receipt_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_delivery_receipt_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated receipt_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextDeliveryReceipt invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.context_id, 'x', sizeof(invalid.context_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_delivery_receipt_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_delivery_receipt_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated context_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextDeliveryReceipt invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.subscription_id, 'x', sizeof(invalid.subscription_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_delivery_receipt_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_delivery_receipt_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated subscription_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextDeliveryReceipt invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.application_id, 'x', sizeof(invalid.application_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_delivery_receipt_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_delivery_receipt_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated application_id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiContextDeliveryReceipt invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.panel_id, 'x', sizeof(invalid.panel_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_context_delivery_receipt_validate(&invalid) != UMI_STATUS_OK) ||
            umi_context_delivery_receipt_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated panel_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiContextDeliveryReceiptTransferCases, UmiContextDeliveryReceipt,
    umi_context_delivery_receipt_archive_encode, umi_context_delivery_receipt_archive_decode,
    UmiContextDeliveryReceiptTransferEqual, UmiContextDeliveryReceiptTransferTails, UmiContextDeliveryReceiptTransferMalformed)

int main(void)
{
    UmiContextDeliveryReceipt value;
    umi_context_delivery_receipt_init(&value);
    value.receipt_id[0] = 's';
    value.context_id[0] = 's';
    value.subscription_id[0] = 's';
    value.application_id[0] = 's';
    value.panel_id[0] = 's';
    value.delivered_at_ms = (uint64_t)17U;
    value.revision = (uint64_t)17U;
    if (umi_context_delivery_receipt_validate(&value) != UMI_STATUS_OK) return 1;
    if (UmiContextDeliveryReceiptTransferCases(&value) != 0) return 1;

    return 0;
}

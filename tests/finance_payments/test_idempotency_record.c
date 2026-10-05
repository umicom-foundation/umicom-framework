/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_idempotency_record.c
 *
 * PURPOSE:
 *   Exercise idempotency record validation and calculations.
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
#include "umicom/finance/payments/idempotency_record.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/idempotency_record.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsIdempotencyRecordTransferEqual(const UmiPaymentsIdempotencyRecord *a, const UmiPaymentsIdempotencyRecord *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->payment_id.value, b->payment_id.value) == 0 &&
        strcmp(a->idempotency_key, b->idempotency_key) == 0 &&
        a->fingerprint == b->fingerprint;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsIdempotencyRecordTransferTails(UmiPaymentsIdempotencyRecord *value)
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
        size_t used = strlen(value->idempotency_key) + 1U;
        memset(value->idempotency_key + used, 0xa5, sizeof(value->idempotency_key) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsIdempotencyRecordTransferMalformed(const UmiPaymentsIdempotencyRecord *sample)
{
    (void)sample;
    {
        UmiPaymentsIdempotencyRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_idempotency_record_valid(&invalid)) ||
            umi_payments_idempotency_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsIdempotencyRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.payment_id.value, 'x', sizeof(invalid.payment_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_idempotency_record_valid(&invalid)) ||
            umi_payments_idempotency_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated payment_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsIdempotencyRecord invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.idempotency_key, 'x', sizeof(invalid.idempotency_key));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_idempotency_record_valid(&invalid)) ||
            umi_payments_idempotency_record_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated idempotency_key was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsIdempotencyRecordTransferCases, UmiPaymentsIdempotencyRecord,
    umi_payments_idempotency_record_archive_encode, umi_payments_idempotency_record_archive_decode,
    UmiPaymentsIdempotencyRecordTransferEqual, UmiPaymentsIdempotencyRecordTransferTails, UmiPaymentsIdempotencyRecordTransferMalformed)

int main(void) {
    UmiPaymentsIdempotencyRecord v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_idempotency_record_init(&v, "idem-rec", "pay-1", "idem-1", 1234U)!=UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_payments_idempotency_record_valid(&v)) return 2;
    if (UmiPaymentsIdempotencyRecordTransferCases(&v) != 0) return 1;

    return 0;
}

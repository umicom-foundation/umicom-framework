/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_instruction.c
 *
 * PURPOSE:
 *   Exercise payment instruction validation and calculations.
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
#include "umicom/finance/payments/payment_instruction.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_instruction.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentInstructionTransferEqual(const UmiPaymentsPaymentInstruction *a, const UmiPaymentsPaymentInstruction *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->debtor_party_id.value, b->debtor_party_id.value) == 0 &&
        strcmp(a->creditor_party_id.value, b->creditor_party_id.value) == 0 &&
        strcmp(a->currency.code, b->currency.code) == 0 &&
        a->amount_minor == b->amount_minor &&
        a->requested_date.year == b->requested_date.year &&
        a->requested_date.month == b->requested_date.month &&
        a->requested_date.day == b->requested_date.day &&
        a->status == b->status &&
        strcmp(a->idempotency_key, b->idempotency_key) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentInstructionTransferTails(UmiPaymentsPaymentInstruction *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->debtor_party_id.value) + 1U;
        memset(value->debtor_party_id.value + used, 0xa5, sizeof(value->debtor_party_id.value) - used);
    }
    {
        size_t used = strlen(value->creditor_party_id.value) + 1U;
        memset(value->creditor_party_id.value + used, 0xa5, sizeof(value->creditor_party_id.value) - used);
    }
    {
        size_t used = strlen(value->currency.code) + 1U;
        memset(value->currency.code + used, 0xa5, sizeof(value->currency.code) - used);
    }
    {
        size_t used = strlen(value->idempotency_key) + 1U;
        memset(value->idempotency_key + used, 0xa5, sizeof(value->idempotency_key) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentInstructionTransferMalformed(const UmiPaymentsPaymentInstruction *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_instruction_valid(&invalid)) ||
            umi_payments_payment_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.debtor_party_id.value, 'x', sizeof(invalid.debtor_party_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_instruction_valid(&invalid)) ||
            umi_payments_payment_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated debtor_party_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.creditor_party_id.value, 'x', sizeof(invalid.creditor_party_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_instruction_valid(&invalid)) ||
            umi_payments_payment_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated creditor_party_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.currency.code, 'x', sizeof(invalid.currency.code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_instruction_valid(&invalid)) ||
            umi_payments_payment_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated currency.code was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentInstruction invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.idempotency_key, 'x', sizeof(invalid.idempotency_key));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_instruction_valid(&invalid)) ||
            umi_payments_payment_instruction_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated idempotency_key was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentInstructionTransferCases, UmiPaymentsPaymentInstruction,
    umi_payments_payment_instruction_archive_encode, umi_payments_payment_instruction_archive_decode,
    UmiPaymentsPaymentInstructionTransferEqual, UmiPaymentsPaymentInstructionTransferTails, UmiPaymentsPaymentInstructionTransferMalformed)

int main(void) {
    UmiPaymentsPaymentInstruction v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_instruction_init(&v, "pay-1", "debtor", "creditor", "GBP", 1500, (UmiFinancialDate){2026,8U,25U}, UMI_PAYMENTS_APPROVED, "idem-1")!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_instruction_valid(&v)) return 2;
    if (UmiPaymentsPaymentInstructionTransferCases(&v) != 0) return 1;

    return 0;
}

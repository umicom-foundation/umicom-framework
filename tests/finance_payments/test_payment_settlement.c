/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_settlement.c
 *
 * PURPOSE:
 *   Exercise payment settlement validation and calculations.
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
#include "umicom/finance/payments/payment_settlement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_settlement.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentSettlementTransferEqual(const UmiPaymentsPaymentSettlement *a, const UmiPaymentsPaymentSettlement *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->payment_id.value, b->payment_id.value) == 0 &&
        strcmp(a->settlement_reference, b->settlement_reference) == 0 &&
        a->amount_minor == b->amount_minor &&
        a->settled == b->settled;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentSettlementTransferTails(UmiPaymentsPaymentSettlement *value)
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
        size_t used = strlen(value->settlement_reference) + 1U;
        memset(value->settlement_reference + used, 0xa5, sizeof(value->settlement_reference) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentSettlementTransferMalformed(const UmiPaymentsPaymentSettlement *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentSettlement invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_settlement_valid(&invalid)) ||
            umi_payments_payment_settlement_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentSettlement invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.payment_id.value, 'x', sizeof(invalid.payment_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_settlement_valid(&invalid)) ||
            umi_payments_payment_settlement_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated payment_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentSettlement invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.settlement_reference, 'x', sizeof(invalid.settlement_reference));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_settlement_valid(&invalid)) ||
            umi_payments_payment_settlement_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated settlement_reference was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentSettlementTransferCases, UmiPaymentsPaymentSettlement,
    umi_payments_payment_settlement_archive_encode, umi_payments_payment_settlement_archive_decode,
    UmiPaymentsPaymentSettlementTransferEqual, UmiPaymentsPaymentSettlementTransferTails, UmiPaymentsPaymentSettlementTransferMalformed)

int main(void) {
    UmiPaymentsPaymentSettlement v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_settlement_init(&v, "settle-1", "pay-1", "SET-1", 1500, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_settlement_valid(&v)) return 2;
    if (UmiPaymentsPaymentSettlementTransferCases(&v) != 0) return 1;

    return 0;
}

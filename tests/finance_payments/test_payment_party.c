/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_party.c
 *
 * PURPOSE:
 *   Exercise payment party validation and calculations.
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
#include "umicom/finance/payments/payment_party.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/payments/payment_party.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiPaymentsPaymentPartyTransferEqual(const UmiPaymentsPaymentParty *a, const UmiPaymentsPaymentParty *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->account_id.value, b->account_id.value) == 0 &&
        strcmp(a->display_name, b->display_name) == 0 &&
        strcmp(a->bank_code, b->bank_code) == 0;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiPaymentsPaymentPartyTransferTails(UmiPaymentsPaymentParty *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->account_id.value) + 1U;
        memset(value->account_id.value + used, 0xa5, sizeof(value->account_id.value) - used);
    }
    {
        size_t used = strlen(value->display_name) + 1U;
        memset(value->display_name + used, 0xa5, sizeof(value->display_name) - used);
    }
    {
        size_t used = strlen(value->bank_code) + 1U;
        memset(value->bank_code + used, 0xa5, sizeof(value->bank_code) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiPaymentsPaymentPartyTransferMalformed(const UmiPaymentsPaymentParty *sample)
{
    (void)sample;
    {
        UmiPaymentsPaymentParty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_party_valid(&invalid)) ||
            umi_payments_payment_party_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentParty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.account_id.value, 'x', sizeof(invalid.account_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_party_valid(&invalid)) ||
            umi_payments_payment_party_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated account_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentParty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.display_name, 'x', sizeof(invalid.display_name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_party_valid(&invalid)) ||
            umi_payments_payment_party_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated display_name was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiPaymentsPaymentParty invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.bank_code, 'x', sizeof(invalid.bank_code));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_payments_payment_party_valid(&invalid)) ||
            umi_payments_payment_party_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated bank_code was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiPaymentsPaymentPartyTransferCases, UmiPaymentsPaymentParty,
    umi_payments_payment_party_archive_encode, umi_payments_payment_party_archive_decode,
    UmiPaymentsPaymentPartyTransferEqual, UmiPaymentsPaymentPartyTransferTails, UmiPaymentsPaymentPartyTransferMalformed)

int main(void) {
    UmiPaymentsPaymentParty v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_party_init(&v, "party-1", "acct-1", "Debtor", "BANKGB2L")!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_party_valid(&v)) return 2;
    if (UmiPaymentsPaymentPartyTransferCases(&v) != 0) return 1;

    return 0;
}

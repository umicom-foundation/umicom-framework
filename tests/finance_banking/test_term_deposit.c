/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_term_deposit.c
 *
 * PURPOSE:
 *   Exercise term deposit validation and calculations.
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
#include "umicom/finance/banking/term_deposit.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/banking/term_deposit.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBankingTermDepositTransferEqual(const UmiBankingTermDeposit *a, const UmiBankingTermDeposit *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->customer_id.value, b->customer_id.value) == 0 &&
        a->principal_minor == b->principal_minor &&
        a->start_date.year == b->start_date.year &&
        a->start_date.month == b->start_date.month &&
        a->start_date.day == b->start_date.day &&
        a->maturity_date.year == b->maturity_date.year &&
        a->maturity_date.month == b->maturity_date.month &&
        a->maturity_date.day == b->maturity_date.day &&
        a->rollover == b->rollover;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBankingTermDepositTransferTails(UmiBankingTermDeposit *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->customer_id.value) + 1U;
        memset(value->customer_id.value + used, 0xa5, sizeof(value->customer_id.value) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBankingTermDepositTransferMalformed(const UmiBankingTermDeposit *sample)
{
    (void)sample;
    {
        UmiBankingTermDeposit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_term_deposit_valid(&invalid)) ||
            umi_banking_term_deposit_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBankingTermDeposit invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.customer_id.value, 'x', sizeof(invalid.customer_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_term_deposit_valid(&invalid)) ||
            umi_banking_term_deposit_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated customer_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBankingTermDepositTransferCases, UmiBankingTermDeposit,
    umi_banking_term_deposit_archive_encode, umi_banking_term_deposit_archive_decode,
    UmiBankingTermDepositTransferEqual, UmiBankingTermDepositTransferTails, UmiBankingTermDepositTransferMalformed)

int main(void) {
    UmiBankingTermDeposit v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_term_deposit_init(&v, "td-1", "cust-1", 100000, (UmiFinancialDate){2026,1U,1U}, (UmiFinancialDate){2027,1U,1U}, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_term_deposit_valid(&v)) return 2;
    if (UmiBankingTermDepositTransferCases(&v) != 0) return 1;

    return 0;
}

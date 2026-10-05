/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_account_hold.c
 *
 * PURPOSE:
 *   Exercise account hold validation and calculations.
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
#include "umicom/finance/banking/account_hold.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/banking/account_hold.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBankingAccountHoldTransferEqual(const UmiBankingAccountHold *a, const UmiBankingAccountHold *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->account_id.value, b->account_id.value) == 0 &&
        a->amount_minor == b->amount_minor &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBankingAccountHoldTransferTails(UmiBankingAccountHold *value)
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
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBankingAccountHoldTransferMalformed(const UmiBankingAccountHold *sample)
{
    (void)sample;
    {
        UmiBankingAccountHold invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_account_hold_valid(&invalid)) ||
            umi_banking_account_hold_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBankingAccountHold invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.account_id.value, 'x', sizeof(invalid.account_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_account_hold_valid(&invalid)) ||
            umi_banking_account_hold_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated account_id.value was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBankingAccountHoldTransferCases, UmiBankingAccountHold,
    umi_banking_account_hold_archive_encode, umi_banking_account_hold_archive_decode,
    UmiBankingAccountHoldTransferEqual, UmiBankingAccountHoldTransferTails, UmiBankingAccountHoldTransferMalformed)

int main(void) {
    UmiBankingAccountHold v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_account_hold_init(&v, "hold-1", "dep-1", 500, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_account_hold_valid(&v)) return 2;
    if (UmiBankingAccountHoldTransferCases(&v) != 0) return 1;

    return 0;
}

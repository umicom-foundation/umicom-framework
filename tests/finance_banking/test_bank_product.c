/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_bank_product.c
 *
 * PURPOSE:
 *   Exercise bank product validation and calculations.
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
#include "umicom/finance/banking/bank_product.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/banking/bank_product.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBankingBankProductTransferEqual(const UmiBankingBankProduct *a, const UmiBankingBankProduct *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->kind == b->kind &&
        a->active == b->active;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBankingBankProductTransferTails(UmiBankingBankProduct *value)
{
    (void)value;
    {
        size_t used = strlen(value->id.value) + 1U;
        memset(value->id.value + used, 0xa5, sizeof(value->id.value) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBankingBankProductTransferMalformed(const UmiBankingBankProduct *sample)
{
    (void)sample;
    {
        UmiBankingBankProduct invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_bank_product_valid(&invalid)) ||
            umi_banking_bank_product_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBankingBankProduct invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_bank_product_valid(&invalid)) ||
            umi_banking_bank_product_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBankingBankProductTransferCases, UmiBankingBankProduct,
    umi_banking_bank_product_archive_encode, umi_banking_bank_product_archive_decode,
    UmiBankingBankProductTransferEqual, UmiBankingBankProductTransferTails, UmiBankingBankProductTransferMalformed)

int main(void) {
    UmiBankingBankProduct v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_bank_product_init(&v, "prod-1", "Current Account", UMI_BANKING_PRODUCT_DEPOSIT, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_bank_product_valid(&v)) return 2;
    if (UmiBankingBankProductTransferCases(&v) != 0) return 1;

    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_bank_relationship.c
 *
 * PURPOSE:
 *   Exercise bank relationship validation and calculations.
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
#include "umicom/finance/banking/bank_relationship.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/banking/bank_relationship.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiBankingBankRelationshipTransferEqual(const UmiBankingBankRelationship *a, const UmiBankingBankRelationship *b)
{
    return strcmp(a->id.value, b->id.value) == 0 &&
        strcmp(a->customer_id.value, b->customer_id.value) == 0 &&
        strcmp(a->relationship_manager, b->relationship_manager) == 0 &&
        a->primary_relationship == b->primary_relationship;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiBankingBankRelationshipTransferTails(UmiBankingBankRelationship *value)
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
    {
        size_t used = strlen(value->relationship_manager) + 1U;
        memset(value->relationship_manager + used, 0xa5, sizeof(value->relationship_manager) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiBankingBankRelationshipTransferMalformed(const UmiBankingBankRelationship *sample)
{
    (void)sample;
    {
        UmiBankingBankRelationship invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id.value, 'x', sizeof(invalid.id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_bank_relationship_valid(&invalid)) ||
            umi_banking_bank_relationship_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBankingBankRelationship invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.customer_id.value, 'x', sizeof(invalid.customer_id.value));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_bank_relationship_valid(&invalid)) ||
            umi_banking_bank_relationship_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated customer_id.value was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiBankingBankRelationship invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.relationship_manager, 'x', sizeof(invalid.relationship_manager));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_banking_bank_relationship_valid(&invalid)) ||
            umi_banking_bank_relationship_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated relationship_manager was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiBankingBankRelationshipTransferCases, UmiBankingBankRelationship,
    umi_banking_bank_relationship_archive_encode, umi_banking_bank_relationship_archive_decode,
    UmiBankingBankRelationshipTransferEqual, UmiBankingBankRelationshipTransferTails, UmiBankingBankRelationshipTransferMalformed)

int main(void) {
    UmiBankingBankRelationship v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_bank_relationship_init(&v, "rel-1", "cust-1", "Manager One", true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_bank_relationship_valid(&v)) return 2;
    if (UmiBankingBankRelationshipTransferCases(&v) != 0) return 1;

    return 0;
}

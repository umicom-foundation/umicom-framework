/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_confirmation_matching.c
 *
 * PURPOSE:
 *   Exercise confirmation matching validation and calculations.
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
#include "umicom/finance/treasury/confirmation_matching.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/treasury/confirmation_matching.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTreasuryConfirmationMatchingTransferEqual(const UmiTreasuryConfirmationMatching *a, const UmiTreasuryConfirmationMatching *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->matched_fields == b->matched_fields &&
        a->total_fields == b->total_fields;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTreasuryConfirmationMatchingTransferTails(UmiTreasuryConfirmationMatching *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTreasuryConfirmationMatchingTransferMalformed(const UmiTreasuryConfirmationMatching *sample)
{
    (void)sample;
    {
        UmiTreasuryConfirmationMatching invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_treasury_confirmation_matching_valid(&invalid)) ||
            umi_treasury_confirmation_matching_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTreasuryConfirmationMatchingTransferCases, UmiTreasuryConfirmationMatching,
    umi_treasury_confirmation_matching_archive_encode, umi_treasury_confirmation_matching_archive_decode,
    UmiTreasuryConfirmationMatchingTransferEqual, UmiTreasuryConfirmationMatchingTransferTails, UmiTreasuryConfirmationMatchingTransferMalformed)

int main(void) {
    UmiTreasuryConfirmationMatching v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_confirmation_matching_init(&v, "match", 8U, 8U) != UMI_STATUS_OK) return 1;
    if (UmiTreasuryConfirmationMatchingTransferCases(&v) != 0) return 1;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_treasury_confirmation_matching_exact(&v))return 2;
    return 0;
}

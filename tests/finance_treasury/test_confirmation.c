/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_confirmation.c
 *
 * PURPOSE:
 *   Exercise confirmation validation and calculations.
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
#include "umicom/finance/treasury/confirmation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/treasury/confirmation.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTreasuryConfirmationTransferEqual(const UmiTreasuryConfirmation *a, const UmiTreasuryConfirmation *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->trade_id, b->trade_id) == 0 &&
        a->sent == b->sent &&
        a->acknowledged == b->acknowledged;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTreasuryConfirmationTransferTails(UmiTreasuryConfirmation *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->trade_id) + 1U;
        memset(value->trade_id + used, 0xa5, sizeof(value->trade_id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTreasuryConfirmationTransferMalformed(const UmiTreasuryConfirmation *sample)
{
    (void)sample;
    {
        UmiTreasuryConfirmation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_treasury_confirmation_valid(&invalid)) ||
            umi_treasury_confirmation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiTreasuryConfirmation invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.trade_id, 'x', sizeof(invalid.trade_id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_treasury_confirmation_valid(&invalid)) ||
            umi_treasury_confirmation_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated trade_id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTreasuryConfirmationTransferCases, UmiTreasuryConfirmation,
    umi_treasury_confirmation_archive_encode, umi_treasury_confirmation_archive_decode,
    UmiTreasuryConfirmationTransferEqual, UmiTreasuryConfirmationTransferTails, UmiTreasuryConfirmationTransferMalformed)

int main(void) {
    UmiTreasuryConfirmation v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_confirmation_init(&v, "conf", "trade-1", true, true) != UMI_STATUS_OK) return 1;
    if (UmiTreasuryConfirmationTransferCases(&v) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_treasury_confirmation_complete(&v))return 2;
    return 0;
}

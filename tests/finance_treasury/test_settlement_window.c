/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_settlement_window.c
 *
 * PURPOSE:
 *   Exercise settlement window validation and calculations.
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
#include "umicom/finance/treasury/settlement_window.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/treasury/settlement_window.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTreasurySettlementWindowTransferEqual(const UmiTreasurySettlementWindow *a, const UmiTreasurySettlementWindow *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->opens_epoch_millis == b->opens_epoch_millis &&
        a->closes_epoch_millis == b->closes_epoch_millis;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTreasurySettlementWindowTransferTails(UmiTreasurySettlementWindow *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTreasurySettlementWindowTransferMalformed(const UmiTreasurySettlementWindow *sample)
{
    (void)sample;
    {
        UmiTreasurySettlementWindow invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_treasury_settlement_window_valid(&invalid)) ||
            umi_treasury_settlement_window_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTreasurySettlementWindowTransferCases, UmiTreasurySettlementWindow,
    umi_treasury_settlement_window_archive_encode, umi_treasury_settlement_window_archive_decode,
    UmiTreasurySettlementWindowTransferEqual, UmiTreasurySettlementWindowTransferTails, UmiTreasurySettlementWindowTransferMalformed)

int main(void) {
    UmiTreasurySettlementWindow v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_settlement_window_init(&v, "window", 100, 250) != UMI_STATUS_OK) return 1;
    if (UmiTreasurySettlementWindowTransferCases(&v) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_settlement_window_duration_millis(&v)!=150)return 2;
    return 0;
}

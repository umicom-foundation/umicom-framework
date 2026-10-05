/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_treasury_snapshot.c
 *
 * PURPOSE:
 *   Exercise treasury snapshot validation and calculations.
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
#include "umicom/finance/treasury/treasury_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/treasury/treasury_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTreasuryTreasurySnapshotTransferEqual(const UmiTreasuryTreasurySnapshot *a, const UmiTreasuryTreasurySnapshot *b)
{
    return strcmp(a->id, b->id) == 0 &&
        a->cash_minor == b->cash_minor &&
        a->liquidity_gap_minor == b->liquidity_gap_minor &&
        a->risk_minor == b->risk_minor &&
        a->collateral_minor == b->collateral_minor;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTreasuryTreasurySnapshotTransferTails(UmiTreasuryTreasurySnapshot *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTreasuryTreasurySnapshotTransferMalformed(const UmiTreasuryTreasurySnapshot *sample)
{
    (void)sample;
    {
        UmiTreasuryTreasurySnapshot invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(!umi_treasury_treasury_snapshot_valid(&invalid)) ||
            umi_treasury_treasury_snapshot_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTreasuryTreasurySnapshotTransferCases, UmiTreasuryTreasurySnapshot,
    umi_treasury_treasury_snapshot_archive_encode, umi_treasury_treasury_snapshot_archive_decode,
    UmiTreasuryTreasurySnapshotTransferEqual, UmiTreasuryTreasurySnapshotTransferTails, UmiTreasuryTreasurySnapshotTransferMalformed)

int main(void) {
    UmiTreasuryTreasurySnapshot v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_treasury_snapshot_init(&v, "snapshot", 1000, -250, 400, 600) != UMI_STATUS_OK) return 1;
    if (UmiTreasuryTreasurySnapshotTransferCases(&v) != 0) return 1;

    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_treasury_snapshot_net_liquidity_minor(&v)!=750)return 2;
    return 0;
}

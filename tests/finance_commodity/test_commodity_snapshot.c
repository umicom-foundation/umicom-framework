/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_commodity_snapshot.c
 *
 * PURPOSE:
 *   Implement the test commodity snapshot behavior for
 *   Umicom Framework.
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
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/commodity_snapshot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/finance/commodity/commodity_snapshot.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiCommoditySnapshotTransferEqual(const UmiCommoditySnapshot *a, const UmiCommoditySnapshot *b)
{
    return a->commodity_count == b->commodity_count &&
        a->contract_count == b->contract_count &&
        a->inventory_count == b->inventory_count &&
        a->shipment_count == b->shipment_count &&
        a->nomination_count == b->nomination_count &&
        a->captured_time_ms == b->captured_time_ms &&
        a->revision == b->revision;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiCommoditySnapshotTransferTails(UmiCommoditySnapshot *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiCommoditySnapshotTransferMalformed(const UmiCommoditySnapshot *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiCommoditySnapshotTransferCases, UmiCommoditySnapshot,
    umi_commodity_commodity_snapshot_archive_encode, umi_commodity_commodity_snapshot_archive_decode,
    UmiCommoditySnapshotTransferEqual, UmiCommoditySnapshotTransferTails, UmiCommoditySnapshotTransferMalformed)

int main(void)
{
    UmiCommoditySnapshot snapshot;
    umi_commodity_commodity_snapshot_init(&snapshot, 1000);
    CHECK(umi_commodity_commodity_snapshot_valid(&snapshot));
    if (UmiCommoditySnapshotTransferCases(&snapshot) != 0) return 1;

    CHECK(snapshot.revision == 1U);
    return 0;
}

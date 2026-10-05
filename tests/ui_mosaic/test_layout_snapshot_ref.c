/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_snapshot_ref.c
 *
 * PURPOSE:
 *   Exercise layout snapshot ref behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/mosaic/layout_snapshot_ref.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/mosaic/layout_snapshot_ref.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiUiMosaicLayoutSnapshotRefTransferEqual(const UmiUiMosaicLayoutSnapshotRef *a, const UmiUiMosaicLayoutSnapshotRef *b)
{
    return strcmp(a->id, b->id) == 0 &&
        strcmp(a->name, b->name) == 0 &&
        a->revision == b->revision &&
        a->item_count == b->item_count &&
        a->locked == b->locked;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiUiMosaicLayoutSnapshotRefTransferTails(UmiUiMosaicLayoutSnapshotRef *value)
{
    (void)value;
    {
        size_t used = strlen(value->id) + 1U;
        memset(value->id + used, 0xa5, sizeof(value->id) - used);
    }
    {
        size_t used = strlen(value->name) + 1U;
        memset(value->name + used, 0xa5, sizeof(value->name) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiUiMosaicLayoutSnapshotRefTransferMalformed(const UmiUiMosaicLayoutSnapshotRef *sample)
{
    (void)sample;
    {
        UmiUiMosaicLayoutSnapshotRef invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.id, 'x', sizeof(invalid.id));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_layout_snapshot_ref_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_layout_snapshot_ref_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated id was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiUiMosaicLayoutSnapshotRef invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.name, 'x', sizeof(invalid.name));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_ui_mosaic_layout_snapshot_ref_validate(&invalid) != UMI_STATUS_OK) ||
            umi_ui_mosaic_layout_snapshot_ref_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated name was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiUiMosaicLayoutSnapshotRefTransferCases, UmiUiMosaicLayoutSnapshotRef,
    umi_ui_mosaic_layout_snapshot_ref_archive_encode, umi_ui_mosaic_layout_snapshot_ref_archive_decode,
    UmiUiMosaicLayoutSnapshotRefTransferEqual, UmiUiMosaicLayoutSnapshotRefTransferTails, UmiUiMosaicLayoutSnapshotRefTransferMalformed)

int main(void) {
    UmiUiMosaicLayoutSnapshotRef value;
    umi_ui_mosaic_layout_snapshot_ref_init(&value);
    CHECK(umi_ui_mosaic_layout_snapshot_ref_set(&value, "layout.layout_snapshot_ref", "Layout Snapshot Ref") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_snapshot_ref_validate(&value) == UMI_STATUS_OK);
    if (UmiUiMosaicLayoutSnapshotRefTransferCases(&value) != 0) return 1;

    CHECK(umi_ui_mosaic_layout_snapshot_ref_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}

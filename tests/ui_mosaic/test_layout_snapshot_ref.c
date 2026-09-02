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
#include "umicom/ui/mosaic/layout_snapshot_ref.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicLayoutSnapshotRef value;
    umi_ui_mosaic_layout_snapshot_ref_init(&value);
    CHECK(umi_ui_mosaic_layout_snapshot_ref_set(&value, "layout.layout_snapshot_ref", "Layout Snapshot Ref") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_snapshot_ref_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_snapshot_ref_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}

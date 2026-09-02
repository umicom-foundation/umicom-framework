/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_lock_state.c
 *
 * PURPOSE:
 *   Exercise layout lock state behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_lock_state.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiUiMosaicLayoutLockState value;
    umi_ui_mosaic_layout_lock_state_init(&value);
    CHECK(umi_ui_mosaic_layout_lock_state_set(&value, "layout.layout_lock_state", "Layout Lock State") == UMI_STATUS_OK);
    value.item_count = 4U;
    CHECK(umi_ui_mosaic_layout_lock_state_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_lock_state_touch(&value) == UMI_STATUS_OK);
    CHECK(value.revision == 2U);
    return 0;
}

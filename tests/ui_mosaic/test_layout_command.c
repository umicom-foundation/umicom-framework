/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_command.c
 *
 * PURPOSE:
 *   Exercise layout command behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_command.h"
int main(void) {
    UmiUiMosaicLayoutCommand value;
    umi_ui_mosaic_layout_command_init(&value);
    CHECK(umi_ui_mosaic_layout_command_set(&value, "layout.command.layout_command", "Layout Command") == UMI_STATUS_OK);
    value.requires_edit_mode = true;
    CHECK(umi_ui_mosaic_layout_command_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_command_can_execute(&value, UMI_UI_MOSAIC_EDIT_LOCKED) == 0);
    CHECK(umi_ui_mosaic_layout_command_can_execute(&value, UMI_UI_MOSAIC_EDIT_UNLOCKED) == 1);
    return 0;
}

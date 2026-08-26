/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_layout_command_set.c
 *
 * PURPOSE:
 *   Exercise layout command set behaviour and invariants.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include "umicom/ui/mosaic/layout_command_set.h"
int main(void) {
    UmiUiMosaicLayoutCommandSet value;
    umi_ui_mosaic_layout_command_set_init(&value);
    CHECK(umi_ui_mosaic_layout_command_set_set(&value, "layout.command.layout_command_set", "Layout Command Set") == UMI_STATUS_OK);
    value.requires_edit_mode = true;
    CHECK(umi_ui_mosaic_layout_command_set_validate(&value) == UMI_STATUS_OK);
    CHECK(umi_ui_mosaic_layout_command_set_can_execute(&value, UMI_UI_MOSAIC_EDIT_LOCKED) == 0);
    CHECK(umi_ui_mosaic_layout_command_set_can_execute(&value, UMI_UI_MOSAIC_EDIT_UNLOCKED) == 1);
    return 0;
}

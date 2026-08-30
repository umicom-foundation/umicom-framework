/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_mosaic/test_types.c
 *
 * PURPOSE:
 *   Exercise types behaviour and invariants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)
#include <string.h>
#include "umicom/ui/mosaic/types.h"
int main(void) {
    char text[16];
    CHECK(umi_ui_mosaic_copy_text(text, sizeof(text), "panel.one") == UMI_STATUS_OK);
    CHECK(strcmp(text, "panel.one") == 0);
    CHECK(umi_ui_mosaic_id_is_valid("studio.editor") == 1);
    CHECK(umi_ui_mosaic_id_is_valid("bad id") == 0);
    CHECK(strcmp(umi_ui_mosaic_application_name(UMI_UI_MOSAIC_APP_TRADER), "Trader") == 0);
    CHECK(strcmp(umi_ui_mosaic_dock_zone_name(UMI_UI_MOSAIC_DOCK_LEFT), "left") == 0);
    return 0;
}

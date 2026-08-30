/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_palette_model.c
 *
 * PURPOSE:
 *   Implement the test palette model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/ui/workstation/palette_model.h"
int main(void) {
    UmiWsPaletteModel m;
    umi_ws_palette_model_init(&m);
    if (umi_ws_palette_model_add(&m, "button", "Button", "Controls") != UMI_STATUS_OK) return 1;
    if (umi_ws_palette_model_add(&m, "grid", "Grid", "Containers") != UMI_STATUS_OK) return 2;
    if (umi_ws_palette_model_count_matching(&m, "Control") != 1U) return 3;
    puts("palette model: ok");
    return 0;
}

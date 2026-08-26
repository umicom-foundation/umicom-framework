/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_types.c
 *
 * PURPOSE:
 *   Verify stable appearance utilities and bounds.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/types.h"
#include <string.h>
int main(void) {
    char text[16]; double scale = 0.0;
    if (umi_appearance_copy_text(text, sizeof text, "theme") != UMI_STATUS_OK) return 1;
    if (strcmp(text, "theme") != 0) return 2;
    if (!umi_appearance_id_valid("studio.theme")) return 3;
    if (umi_appearance_clamp_scale(4.0, 0.5, 3.0, &scale) != UMI_STATUS_OK || scale != 3.0) return 4;
    return 0;
}

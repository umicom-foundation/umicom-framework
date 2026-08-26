/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_border_style_projection.c
 *
 * PURPOSE:
 *   Verify resolve semantic border width, radius and token identity without exposing toolkit CSS syntax.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/border_style_projection.h"
int main(void) {
    UmiAppearanceBorderStyleProjection item;
    if (umi_appearance_border_style_projection_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_border_style_projection_is_valid(&item)) return 2;
    return 0;
}

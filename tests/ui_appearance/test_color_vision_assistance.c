/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_color_vision_assistance.c
 *
 * PURPOSE:
 *   Verify require non-colour cues when semantic state could otherwise depend on colour perception alone.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/color_vision_assistance.h"
int main(void) {
    UmiAppearanceColorVisionAssistance item;
    if (umi_appearance_color_vision_assistance_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_color_vision_assistance_is_valid(&item)) return 2;
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_renderer_appearance_capability.c
 *
 * PURPOSE:
 *   Verify declare appearance capabilities and limitations for GTK4, Qt6, Native Web or headless renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_appearance_capability.h"
int main(void) {
    UmiAppearanceRendererAppearanceCapability item;
    if (umi_appearance_renderer_appearance_capability_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_renderer_appearance_capability_is_valid(&item)) return 2;
    return 0;
}

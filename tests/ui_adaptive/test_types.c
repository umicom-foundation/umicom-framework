/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_types.c
 *
 * PURPOSE:
 *   Validate adaptive shared values and bounded helper behaviour.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/adaptive/types.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    char text[16];
    UmiAdaptiveViewport viewport = { 1440, 900 };
    CHECK(umi_adaptive_copy_text(text, sizeof text, "desktop") == UMI_STATUS_OK);
    CHECK(strcmp(text, "desktop") == 0);
    CHECK(strcmp(umi_adaptive_device_family_name(UMI_ADAPTIVE_DEVICE_TABLET), "tablet") == 0);
    CHECK(umi_adaptive_orientation_from_viewport(viewport) == UMI_ADAPTIVE_ORIENTATION_LANDSCAPE);
    return 0;
}

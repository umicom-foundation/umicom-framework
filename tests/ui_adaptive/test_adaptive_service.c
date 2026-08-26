/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_adaptive_service.c
 *
 * PURPOSE:
 *   Validate expose aggregate adaptive-shell readiness and active-device state to framework applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/adaptive_service.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveShellLayout layout; UmiAdaptiveShellProfile profile; UmiAdaptiveServiceSnapshot snap; UmiAdaptiveViewport v={1440,900};
    CHECK(umi_adaptive_shell_layout_init(&layout,"main")==UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_profile_init(&profile,"desktop",UMI_ADAPTIVE_DEVICE_LAPTOP,UMI_ADAPTIVE_NAV_TOP_BAR,UMI_DESIGN_DENSITY_STANDARD,&layout)==UMI_STATUS_OK);
    CHECK(umi_adaptive_service_snapshot(&profile,v,UMI_ADAPTIVE_INPUT_POINTER,&snap)==UMI_STATUS_OK);
    CHECK(snap.ready);
    return 0;
}

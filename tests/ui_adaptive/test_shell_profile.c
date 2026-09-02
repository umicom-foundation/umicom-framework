/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_shell_profile.c
 *
 * PURPOSE:
 *   Validate bind a semantic shell layout to device, density and navigation preferences.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_profile.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveShellLayout layout;
    UmiAdaptiveShellProfile profile;
    CHECK(umi_adaptive_shell_layout_init(&layout, "main") == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_profile_init(&profile, "desktop", UMI_ADAPTIVE_DEVICE_DESKTOP, UMI_ADAPTIVE_NAV_SIDE_RAIL, UMI_DESIGN_DENSITY_COMPACT, &layout) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_profile_valid(&profile));
    return 0;
}

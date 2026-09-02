/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_shell_resolver.c
 *
 * PURPOSE:
 *   Validate resolve a shell profile into concrete region presentations for one viewport class.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_resolver.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveResponsiveRegion responsive;
    UmiAdaptiveShellRegion region;
    UmiAdaptiveShellLayout layout;
    UmiAdaptiveShellProfile profile;
    UmiAdaptiveResolvedShell resolved;
    CHECK(umi_adaptive_responsive_region_init(&responsive, "nav", UMI_ADAPTIVE_REGION_NAVIGATION, UMI_DESIGN_SIZE_COMPACT, UMI_DESIGN_SIZE_WIDE, 10U) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_region_init(&region, &responsive, 280, 1, 1) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_layout_init(&layout, "main") == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_layout_add(&layout, &region) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_profile_init(&profile, "adaptive", UMI_ADAPTIVE_DEVICE_TABLET, UMI_ADAPTIVE_NAV_DRAWER, UMI_DESIGN_DENSITY_TOUCH, &layout) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_resolver_resolve(&profile, UMI_DESIGN_SIZE_COMPACT, &resolved) == UMI_STATUS_OK);
    CHECK(resolved.overlay_regions == 1U);
    return 0;
}

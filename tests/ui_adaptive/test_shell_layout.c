/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_shell_layout.c
 *
 * PURPOSE:
 *   Validate hold a bounded semantic application-shell layout shared by all frontends.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/shell_layout.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiAdaptiveResponsiveRegion responsive;
    UmiAdaptiveShellRegion region;
    UmiAdaptiveShellLayout layout;
    CHECK(umi_adaptive_responsive_region_init(&responsive, "primary", UMI_ADAPTIVE_REGION_PRIMARY, UMI_DESIGN_SIZE_COMPACT, UMI_DESIGN_SIZE_WIDE, 100U) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_region_init(&region, &responsive, 0, 0, 0) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_layout_init(&layout, "main") == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_layout_add(&layout, &region) == UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_layout_find(&layout, "primary") != NULL);
    return 0;
}

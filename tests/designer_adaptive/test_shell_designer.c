/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_shell_designer.c
 *
 * PURPOSE:
 *   Validate own designer selection and dirty-state for adaptive application-shell regions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/shell_designer.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveResponsiveRegion rr; UmiAdaptiveShellRegion sr; UmiAdaptiveShellLayout l; UmiAdaptiveShellProfile p; UmiDesignerShellDesigner d;
    CHECK(umi_adaptive_responsive_region_init(&rr,"primary",UMI_ADAPTIVE_REGION_PRIMARY,UMI_DESIGN_SIZE_COMPACT,UMI_DESIGN_SIZE_WIDE,1U)==UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_region_init(&sr,&rr,0,0,0)==UMI_STATUS_OK); CHECK(umi_adaptive_shell_layout_init(&l,"l")==UMI_STATUS_OK); CHECK(umi_adaptive_shell_layout_add(&l,&sr)==UMI_STATUS_OK);
    CHECK(umi_adaptive_shell_profile_init(&p,"p",UMI_ADAPTIVE_DEVICE_DESKTOP,UMI_ADAPTIVE_NAV_TOP_BAR,UMI_DESIGN_DENSITY_STANDARD,&l)==UMI_STATUS_OK);
    CHECK(umi_designer_shell_designer_init(&d,&p)==UMI_STATUS_OK); CHECK(umi_designer_shell_designer_select_region(&d,"primary")==UMI_STATUS_OK); CHECK(d.dirty);
    return 0;
}

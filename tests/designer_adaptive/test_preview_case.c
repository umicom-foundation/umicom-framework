/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_preview_case.c
 *
 * PURPOSE:
 *   Validate define one expected responsive preview case for deterministic designer verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/preview_case.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerResponsivePreview p; UmiDesignerPreviewCase c; UmiAdaptiveViewport v={390,844}; UmiAdaptiveInsets s={0,0,0,0};
    CHECK(umi_designer_responsive_preview_init(&p,"p",v,s,UMI_ADAPTIVE_INPUT_TOUCH,UMI_RAD_TARGET_GTK4)==UMI_STATUS_OK);
    CHECK(umi_designer_preview_case_init(&c,"compact",&p,UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_NAV_BOTTOM_BAR)==UMI_STATUS_OK);
    CHECK(c.expected_size_class==UMI_DESIGN_SIZE_COMPACT);
    return 0;
}

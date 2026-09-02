/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_accessibility_scale.c
 *
 * PURPOSE:
 *   Validate combine user text, target-size and contrast preferences for adaptive design previews.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/accessibility_scale.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerAccessibilityScale s;
    CHECK(umi_designer_accessibility_scale_init(&s,1.5,1.25,1)==UMI_STATUS_OK);
    CHECK(umi_designer_accessibility_scale_target(&s,40)==50);
    return 0;
}

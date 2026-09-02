/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_navigation_pattern.c
 *
 * PURPOSE:
 *   Validate select an adaptive navigation pattern from canonical size class and input mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/navigation_pattern.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    CHECK(umi_adaptive_navigation_pattern_select(UMI_DESIGN_SIZE_COMPACT, UMI_ADAPTIVE_INPUT_TOUCH) == UMI_ADAPTIVE_NAV_BOTTOM_BAR);
    CHECK(umi_adaptive_navigation_pattern_select(UMI_DESIGN_SIZE_WIDE, UMI_ADAPTIVE_INPUT_POINTER) == UMI_ADAPTIVE_NAV_SIDE_RAIL);
    return 0;
}

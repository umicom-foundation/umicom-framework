/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/navigation_pattern.c
 *
 * PURPOSE:
 *   Select an adaptive navigation pattern from canonical size class and input mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/navigation_pattern.h"

/* Use mobile-friendly bottom navigation for compact touch and rail navigation for larger views. */
UmiAdaptiveNavigationPattern umi_adaptive_navigation_pattern_select(UmiDesignSizeClass size_class,
                                                                    UmiAdaptiveInputMode input_mode)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (size_class == UMI_DESIGN_SIZE_COMPACT) {
        return input_mode == UMI_ADAPTIVE_INPUT_TOUCH
            ? UMI_ADAPTIVE_NAV_BOTTOM_BAR
            : UMI_ADAPTIVE_NAV_DRAWER;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (size_class == UMI_DESIGN_SIZE_MEDIUM) {
        return UMI_ADAPTIVE_NAV_DRAWER;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (size_class == UMI_DESIGN_SIZE_WIDE) {
        return UMI_ADAPTIVE_NAV_SIDE_RAIL;
    }
    return UMI_ADAPTIVE_NAV_TOP_BAR;
}

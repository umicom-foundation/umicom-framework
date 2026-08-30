/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/multi_window_policy.c
 *
 * PURPOSE:
 *   Govern detachable and multi-window application surfaces by device family and available width.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/multi_window_policy.h"

/* Restrict multi-window workflows on handset/tablet contexts and enable them on workstations. */
UmiAdaptiveMultiWindowPolicy umi_adaptive_multi_window_policy_resolve(UmiAdaptiveDeviceFamily family,
                                                                      UmiDesignSizeClass size_class)
{
    UmiAdaptiveMultiWindowPolicy policy;
    policy.allow_detach = 0;
    policy.allow_multiple_top_level = 0;
    policy.maximum_windows = 1U;
    if ((family == UMI_ADAPTIVE_DEVICE_DESKTOP || family == UMI_ADAPTIVE_DEVICE_LARGE_DISPLAY) &&
        size_class >= UMI_DESIGN_SIZE_EXPANDED) {
        policy.allow_detach = 1;
        policy.allow_multiple_top_level = 1;
        policy.maximum_windows = 16U;
    } else if (family == UMI_ADAPTIVE_DEVICE_LAPTOP && size_class >= UMI_DESIGN_SIZE_EXPANDED) {
        policy.allow_detach = 1;
        policy.maximum_windows = 4U;
    }
    return policy;
}

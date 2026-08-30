/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/density_policy.c
 *
 * PURPOSE:
 *   Choose canonical Design-System density from viewport and input capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/density_policy.h"

/* Prefer touch density for touch-first contexts and compact density for wide workstations. */
UmiStatus umi_adaptive_density_policy_resolve(UmiDesignSizeClass size_class,
                                              UmiAdaptiveInputMode input_mode,
                                              UmiDesignDensity *out_density)
{
    if (out_density == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (input_mode == UMI_ADAPTIVE_INPUT_TOUCH) {
        *out_density = UMI_DESIGN_DENSITY_TOUCH;
    } else if (size_class == UMI_DESIGN_SIZE_WIDE) {
        *out_density = UMI_DESIGN_DENSITY_COMPACT;
    } else if (input_mode == UMI_ADAPTIVE_INPUT_HYBRID) {
        *out_density = UMI_DESIGN_DENSITY_STANDARD;
    } else {
        *out_density = UMI_DESIGN_DENSITY_STANDARD;
    }
    return UMI_STATUS_OK;
}

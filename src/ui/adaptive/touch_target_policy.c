/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/touch_target_policy.c
 *
 * PURPOSE:
 *   Resolve accessible minimum interactive target dimensions for pointer and touch inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/touch_target_policy.h"

/* Keep touch targets at least 44 logical units while allowing compact pointer workstations. */
int32_t umi_adaptive_touch_target_policy_minimum(UmiAdaptiveInputMode input_mode,
                                                 UmiDesignDensity density)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (input_mode == UMI_ADAPTIVE_INPUT_TOUCH || input_mode == UMI_ADAPTIVE_INPUT_HYBRID) return 44;
    /* Apply this branch only when its contract condition is satisfied. */
    if (density == UMI_DESIGN_DENSITY_COMPACT) return 28;
    return 32;
}

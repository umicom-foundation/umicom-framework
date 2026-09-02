/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/density_profile.c
 *
 * PURPOSE:
 *   Implement semantic spacing, row height and hit-target metrics for Compact, Comfortable and Touch workstation densities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/density_profile.h"

/*
 * Provide the ws density profile make operation used by this module and its client
 * applications.
 */
UmiWsDensityProfile umi_ws_density_profile_make(UmiWsDensity density) {
    UmiWsDensityProfile profile = {UMI_WS_DENSITY_COMFORTABLE, 8, 32, 40, 28, 32};
    /* Apply this branch only when its contract condition is satisfied. */
    if (density == UMI_WS_DENSITY_COMPACT) {
        profile = (UmiWsDensityProfile){density, 5, 28, 34, 23, 28};
    } else /* Apply this branch only when its contract condition is satisfied. */ if (density == UMI_WS_DENSITY_TOUCH) {
        profile = (UmiWsDensityProfile){density, 12, 44, 52, 42, 48};
    }
    return profile;
}

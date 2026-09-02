/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/workstation/accessibility_profile.c
 *
 * PURPOSE:
 *   Implement workstation accessibility requirements for keyboard docking, labels, target sizes and reduced motion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/workstation/accessibility_profile.h"
#include "umicom/ui/workstation/density_profile.h"

/*
 * Provide the ws accessibility profile default operation used by this module and its
 * client applications.
 */
UmiWsAccessibilityProfile umi_ws_accessibility_profile_default(UmiWsDensity density) {
    UmiWsDensityProfile metrics = umi_ws_density_profile_make(density);
    UmiWsAccessibilityProfile profile = {true, true, false, false, metrics.hit_target};
    return profile;
}

/*
 * Provide the ws accessibility profile hit target ok operation used by this module and its
 * client applications.
 */
bool umi_ws_accessibility_profile_hit_target_ok(const UmiWsAccessibilityProfile *profile, int32_t width, int32_t height) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) return false;
    return width >= profile->minimum_hit_target && height >= profile->minimum_hit_target;
}

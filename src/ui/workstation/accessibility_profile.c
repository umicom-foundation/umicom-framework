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

UmiWsAccessibilityProfile umi_ws_accessibility_profile_default(UmiWsDensity density) {
    UmiWsDensityProfile metrics = umi_ws_density_profile_make(density);
    UmiWsAccessibilityProfile profile = {true, true, false, false, metrics.hit_target};
    return profile;
}

bool umi_ws_accessibility_profile_hit_target_ok(const UmiWsAccessibilityProfile *profile, int32_t width, int32_t height) {
    if (profile == NULL) return false;
    return width >= profile->minimum_hit_target && height >= profile->minimum_hit_target;
}

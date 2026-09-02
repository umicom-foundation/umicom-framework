/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/accessibility_profile.h
 *
 * PURPOSE:
 *   Define workstation accessibility requirements for keyboard docking, labels, target sizes and reduced motion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_ACCESSIBILITY_PROFILE_H
#define UMICOM_UI_WORKSTATION_ACCESSIBILITY_PROFILE_H

#include <stdbool.h>
#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ws accessibility profile data shared with callers of this public contract.
 */
typedef struct UmiWsAccessibilityProfile {
    bool keyboard_docking;
    bool require_non_colour_group_label;
    bool reduced_motion;
    bool high_contrast;
    int32_t minimum_hit_target;
} UmiWsAccessibilityProfile;

/**
 * Provide the ws accessibility profile default operation used by this module and its
 * client applications.
 */
UmiWsAccessibilityProfile umi_ws_accessibility_profile_default(UmiWsDensity density);
/**
 * Provide the ws accessibility profile hit target ok operation used by this module and its
 * client applications.
 */
bool umi_ws_accessibility_profile_hit_target_ok(const UmiWsAccessibilityProfile *profile, int32_t width, int32_t height);

#ifdef __cplusplus
}
#endif

#endif

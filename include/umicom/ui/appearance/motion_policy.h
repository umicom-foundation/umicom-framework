/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/motion_policy.h
 *
 * PURPOSE:
 *   Define semantic motion allowances and maximum transition durations for production UI.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_MOTION_POLICY_H
#define UMICOM_UI_APPEARANCE_MOTION_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceMotionPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t standard_duration_ms;
    uint32_t emphasis_duration_ms;
    bool allow_decorative_motion;
    bool allow_parallax;
} UmiAppearanceMotionPolicy;

/* Initialise one motion policy record with deterministic defaults. */
UmiStatus umi_appearance_motion_policy_init(UmiAppearanceMotionPolicy *item);
/* Validate the required production invariants for this motion policy. */
int umi_appearance_motion_policy_is_valid(const UmiAppearanceMotionPolicy *item);

#ifdef __cplusplus
}
#endif
#endif

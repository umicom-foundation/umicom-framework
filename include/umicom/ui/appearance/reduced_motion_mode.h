/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/reduced_motion_mode.h
 *
 * PURPOSE:
 *   Resolve reduced-motion presentation requirements from user and system accessibility settings.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_REDUCED_MOTION_MODE_H
#define UMICOM_UI_APPEARANCE_REDUCED_MOTION_MODE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceReducedMotionMode {
    char mode_id[UMI_APPEARANCE_ID_CAPACITY];
    bool enabled;
    uint32_t maximum_duration_ms;
    bool disable_decorative;
    bool preserve_essential_feedback;
} UmiAppearanceReducedMotionMode;

/* Initialise one reduced motion mode record with deterministic defaults. */
UmiStatus umi_appearance_reduced_motion_mode_init(UmiAppearanceReducedMotionMode *item);
/* Validate the required production invariants for this reduced motion mode. */
int umi_appearance_reduced_motion_mode_is_valid(const UmiAppearanceReducedMotionMode *item);

#ifdef __cplusplus
}
#endif
#endif

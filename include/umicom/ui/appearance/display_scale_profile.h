/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/display_scale_profile.h
 *
 * PURPOSE:
 *   Combine display DPI, operating-system scale and user accessibility scale into one profile.
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
#ifndef UMICOM_UI_APPEARANCE_DISPLAY_SCALE_PROFILE_H
#define UMICOM_UI_APPEARANCE_DISPLAY_SCALE_PROFILE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceDisplayScaleProfile {
    char display_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t dpi;
    double os_scale;
    double user_scale;
    double effective_scale;
} UmiAppearanceDisplayScaleProfile;

/* Initialise one display scale profile record with deterministic defaults. */
UmiStatus umi_appearance_display_scale_profile_init(UmiAppearanceDisplayScaleProfile *item);
/* Validate the required production invariants for this display scale profile. */
int umi_appearance_display_scale_profile_is_valid(const UmiAppearanceDisplayScaleProfile *item);
/* Recalculate the effective display scale after OS or user scale changes. */
UmiStatus umi_appearance_display_scale_profile_resolve(UmiAppearanceDisplayScaleProfile *item);

#ifdef __cplusplus
}
#endif
#endif

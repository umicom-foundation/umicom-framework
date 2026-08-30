/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_profile.h
 *
 * PURPOSE:
 *   Capture a resolved user/application appearance profile shared by every renderer.
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
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_PROFILE_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_PROFILE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceAppearanceProfile {
    char profile_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignThemeMode theme_mode;
    UmiDesignDensity density;
    double text_scale;
    double display_scale;
    bool reduced_motion;
    bool high_contrast;
} UmiAppearanceAppearanceProfile;

/* Initialise one appearance profile record with deterministic defaults. */
UmiStatus umi_appearance_profile_init(UmiAppearanceAppearanceProfile *item);
/* Validate the required production invariants for this appearance profile. */
int umi_appearance_profile_is_valid(const UmiAppearanceAppearanceProfile *item);

#ifdef __cplusplus
}
#endif
#endif

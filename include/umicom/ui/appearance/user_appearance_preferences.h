/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/user_appearance_preferences.h
 *
 * PURPOSE:
 *   Capture user-selected theme, density, motion and text-scale preferences independently of toolkit settings.
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
#ifndef UMICOM_UI_APPEARANCE_USER_APPEARANCE_PREFERENCES_H
#define UMICOM_UI_APPEARANCE_USER_APPEARANCE_PREFERENCES_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance user appearance preferences data shared with callers of this
 * public contract.
 */
typedef struct UmiAppearanceUserAppearancePreferences {
    char user_scope_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignThemeMode theme_mode;
    UmiDesignDensity density;
    double text_scale;
    bool follow_system_theme;
    bool reduced_motion;
    bool high_contrast;
} UmiAppearanceUserAppearancePreferences;

/* Initialise one user appearance preferences record with deterministic defaults. */
UmiStatus umi_appearance_user_appearance_preferences_init(UmiAppearanceUserAppearancePreferences *item);
/* Validate the required production invariants for this user appearance preferences. */
int umi_appearance_user_appearance_preferences_is_valid(const UmiAppearanceUserAppearancePreferences *item);

#ifdef __cplusplus
}
#endif
#endif

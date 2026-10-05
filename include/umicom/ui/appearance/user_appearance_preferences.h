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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_user_appearance_preferences_archive_encode(const UmiAppearanceUserAppearancePreferences *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_user_appearance_preferences_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceUserAppearancePreferences *value);

#ifdef __cplusplus
}
#endif
#endif

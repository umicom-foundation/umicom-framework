/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/user_appearance_preferences.c
 *
 * PURPOSE:
 *   Capture user-selected theme, density, motion and text-scale preferences independently of toolkit settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/user_appearance_preferences.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_user_appearance_preferences_init(UmiAppearanceUserAppearancePreferences *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->user_scope_id,sizeof item->user_scope_id,"user.default");
    item->theme_mode=UMI_DESIGN_THEME_DARK;
    item->density=UMI_DESIGN_DENSITY_STANDARD;
    item->text_scale=1.0;
    item->follow_system_theme=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_user_appearance_preferences_is_valid(const UmiAppearanceUserAppearancePreferences *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->user_scope_id) && item->text_scale >= 0.5 && item->text_scale <= 4.0);
}

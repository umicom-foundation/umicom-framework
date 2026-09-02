/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_profile.c
 *
 * PURPOSE:
 *   Capture a resolved user/application appearance profile shared by every renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_profile.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_profile_init(UmiAppearanceAppearanceProfile *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->profile_id, sizeof item->profile_id, "appearance.default");
    item->theme_mode = UMI_DESIGN_THEME_DARK;
    item->density = UMI_DESIGN_DENSITY_STANDARD;
    item->text_scale = 1.0;
    item->display_scale = 1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_profile_is_valid(const UmiAppearanceAppearanceProfile *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->profile_id) && item->text_scale > 0.0 && item->display_scale > 0.0);
}

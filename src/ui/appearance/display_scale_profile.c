/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/display_scale_profile.c
 *
 * PURPOSE:
 *   Combine display DPI, operating-system scale and user accessibility scale into one profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/display_scale_profile.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_display_scale_profile_init(UmiAppearanceDisplayScaleProfile *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->display_id,sizeof item->display_id,"display.primary");
    item->dpi=96U;
    item->os_scale=1.0;
    item->user_scale=1.0;
    item->effective_scale=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_display_scale_profile_is_valid(const UmiAppearanceDisplayScaleProfile *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->display_id) && item->dpi > 0U && item->os_scale > 0.0 && item->user_scale > 0.0 && item->effective_scale > 0.0);
}
UmiStatus umi_appearance_display_scale_profile_resolve(UmiAppearanceDisplayScaleProfile *item){if(item==NULL||item->os_scale<=0.0||item->user_scale<=0.0)return UMI_STATUS_INVALID_ARGUMENT;item->effective_scale=item->os_scale*item->user_scale;return UMI_STATUS_OK;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/high_contrast_mode.c
 *
 * PURPOSE:
 *   Represent high-contrast presentation requirements layered over the canonical Design System.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/high_contrast_mode.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_high_contrast_mode_init(UmiAppearanceHighContrastMode *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->mode_id,sizeof item->mode_id,"high-contrast");
    item->enabled=true;
    item->force_visible_borders=true;
    item->force_focus_outline=true;
    item->minimum_border_width=2.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_high_contrast_mode_is_valid(const UmiAppearanceHighContrastMode *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->mode_id) && item->minimum_border_width >= 0.0);
}

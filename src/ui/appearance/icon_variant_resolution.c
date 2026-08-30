/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_variant_resolution.c
 *
 * PURPOSE:
 *   Resolve light/dark/high-contrast and direction-aware icon variants while preserving semantic identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_variant_resolution.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_icon_variant_resolution_init(UmiAppearanceIconVariantResolution *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->icon_id,sizeof item->icon_id,"navigation.forward");
    (void)umi_appearance_copy_text(item->resolved_variant_id,sizeof item->resolved_variant_id,"navigation.forward.dark");
    item->mode=UMI_DESIGN_THEME_DARK;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_icon_variant_resolution_is_valid(const UmiAppearanceIconVariantResolution *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->icon_id) && umi_appearance_id_valid(item->resolved_variant_id));
}
void umi_appearance_icon_variant_resolution_set_direction(UmiAppearanceIconVariantResolution *item,int rtl,int direction_sensitive){if(item!=NULL){item->rtl=rtl!=0;item->mirrored=item->rtl&&direction_sensitive;}}

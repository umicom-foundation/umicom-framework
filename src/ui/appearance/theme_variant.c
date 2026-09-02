/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_variant.c
 *
 * PURPOSE:
 *   Bind a semantic theme pack to light, dark or high-contrast presentation mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_variant.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_variant_init(UmiAppearanceThemeVariant *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->variant_id,sizeof item->variant_id,"variant.dark");
    (void)umi_appearance_copy_text(item->pack_id,sizeof item->pack_id,"theme.default.dark");
    item->mode=UMI_DESIGN_THEME_DARK;
    item->preferred=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_variant_is_valid(const UmiAppearanceThemeVariant *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->variant_id) && umi_appearance_id_valid(item->pack_id));
}

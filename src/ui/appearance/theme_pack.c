/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_pack.c
 *
 * PURPOSE:
 *   Describe a versionable semantic theme pack without toolkit CSS or widget classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_pack.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_pack_init(UmiAppearanceThemePack *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->pack_id, sizeof item->pack_id, "theme.default.dark");
    (void)umi_appearance_copy_text(item->brand_id, sizeof item->brand_id, "umicom");
    (void)umi_appearance_copy_text(item->token_set_id, sizeof item->token_set_id, "design.tokens.default");
    item->mode = UMI_DESIGN_THEME_DARK;
    item->revision = 1U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_pack_is_valid(const UmiAppearanceThemePack *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->pack_id) && umi_appearance_id_valid(item->token_set_id) && item->revision > 0U);
}

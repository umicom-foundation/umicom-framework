/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/palette_override.c
 *
 * PURPOSE:
 *   Describe a scoped semantic palette override without embedding literal renderer colours.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/palette_override.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_palette_override_init(UmiAppearancePaletteOverride *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->override_id,sizeof item->override_id,"override.accent");
    (void)umi_appearance_copy_text(item->role_id,sizeof item->role_id,"accent.primary");
    (void)umi_appearance_copy_text(item->token_id,sizeof item->token_id,"studio.accent");
    item->scope=UMI_APPEARANCE_SCOPE_APPLICATION;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_palette_override_is_valid(const UmiAppearancePaletteOverride *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->override_id) && umi_appearance_id_valid(item->role_id) && umi_appearance_id_valid(item->token_id));
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/semantic_palette_map.c
 *
 * PURPOSE:
 *   Map a semantic colour role to a Design-System token identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/semantic_palette_map.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_semantic_palette_map_init(UmiAppearanceSemanticPaletteMap *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->role_id,sizeof item->role_id,"surface.background");
    (void)umi_appearance_copy_text(item->token_id,sizeof item->token_id,"color.surface.background");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_semantic_palette_map_is_valid(const UmiAppearanceSemanticPaletteMap *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->role_id) && umi_appearance_id_valid(item->token_id));
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/palette_resolution.c
 *
 * PURPOSE:
 *   Record the winning token for a semantic palette role after scope precedence is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/palette_resolution.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_palette_resolution_init(UmiAppearancePaletteResolution *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->role_id,sizeof item->role_id,"accent.primary");
    (void)umi_appearance_copy_text(item->base_token_id,sizeof item->base_token_id,"accent.default");
    (void)umi_appearance_copy_text(item->resolved_token_id,sizeof item->resolved_token_id,"accent.default");
    item->winning_scope=UMI_APPEARANCE_SCOPE_SYSTEM;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_palette_resolution_is_valid(const UmiAppearancePaletteResolution *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->role_id) && umi_appearance_id_valid(item->resolved_token_id));
}
/*
 * Provide the appearance palette resolution override operation used by this module and its
 * client applications.
 */
UmiStatus umi_appearance_palette_resolution_override(UmiAppearancePaletteResolution *item,const char *token_id,UmiAppearanceScope scope){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_appearance_id_valid(token_id))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_appearance_copy_text(item->resolved_token_id,sizeof item->resolved_token_id,token_id)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;item->winning_scope=scope;return UMI_STATUS_OK;}

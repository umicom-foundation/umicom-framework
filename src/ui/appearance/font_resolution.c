/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_resolution.c
 *
 * PURPOSE:
 *   Record the winning family and fallback depth selected for a semantic font stack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_resolution.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_font_resolution_init(UmiAppearanceFontResolution *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->stack_id,sizeof item->stack_id,"ui");
    (void)umi_appearance_copy_text(item->resolved_family_id,sizeof item->resolved_family_id,"font.primary");
    item->available=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_font_resolution_is_valid(const UmiAppearanceFontResolution *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->stack_id) && (!item->available || umi_appearance_id_valid(item->resolved_family_id)));
}
UmiStatus umi_appearance_font_resolution_choose(UmiAppearanceFontResolution *item,const char *preferred,const char *fallback){const char *chosen; if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; chosen=(preferred&&preferred[0])?preferred:fallback; if(!umi_appearance_id_valid(chosen)){item->available=false;item->resolved_family_id[0]=0;return UMI_STATUS_NOT_FOUND;} item->available=true;item->fallback_depth=(preferred&&preferred[0])?0U:1U;return umi_appearance_copy_text(item->resolved_family_id,sizeof item->resolved_family_id,chosen);}

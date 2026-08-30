/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/alignment_guide.c
 *
 * PURPOSE:
 *   Represent alignment evidence between visual components.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/rad/alignment_guide.h"
#include <string.h>
UmiStatus umi_rad_alignment_guide_init(UmiRadAlignmentGuide *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->source_id, sizeof item->source_id, "alignment_guide");
    (void)umi_rad_copy_text(item->peer_id, sizeof item->peer_id, "alignment_guide");
    return UMI_STATUS_OK;
}
int umi_rad_alignment_guide_is_valid(const UmiRadAlignmentGuide *item){if(item==NULL)return 0;return umi_rad_id_valid(item->source_id) && umi_rad_id_valid(item->peer_id);}

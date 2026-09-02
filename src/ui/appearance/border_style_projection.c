/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/border_style_projection.c
 *
 * PURPOSE:
 *   Resolve semantic border width, radius and token identity without exposing toolkit CSS syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/border_style_projection.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_border_style_projection_init(UmiAppearanceBorderStyleProjection *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->style_id,sizeof item->style_id,"border.panel");
    (void)umi_appearance_copy_text(item->color_token,sizeof item->color_token,"color.border.subtle");
    item->width_dp=1.0;
    item->radius_dp=6.0;
    item->visible=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_border_style_projection_is_valid(const UmiAppearanceBorderStyleProjection *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->style_id) && item->width_dp >= 0.0 && item->radius_dp >= 0.0);
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/surface_style_projection.c
 *
 * PURPOSE:
 *   Resolve semantic surface roles to token identities consumed by frontend renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/surface_style_projection.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_surface_style_projection_init(UmiAppearanceSurfaceStyleProjection *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->surface_id,sizeof item->surface_id,"surface.panel");
    (void)umi_appearance_copy_text(item->background_token,sizeof item->background_token,"color.surface.panel");
    (void)umi_appearance_copy_text(item->foreground_token,sizeof item->foreground_token,"color.text.primary");
    (void)umi_appearance_copy_text(item->border_token,sizeof item->border_token,"color.border.subtle");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_surface_style_projection_is_valid(const UmiAppearanceSurfaceStyleProjection *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->surface_id) && umi_appearance_id_valid(item->background_token) && umi_appearance_id_valid(item->foreground_token));
}

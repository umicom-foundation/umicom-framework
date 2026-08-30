/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/renderer_theme_projection.c
 *
 * PURPOSE:
 *   Record one renderer-specific projection of a semantic style packet without transferring state ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_theme_projection.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_renderer_theme_projection_init(UmiAppearanceRendererThemeProjection *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->projection_id,sizeof item->projection_id,"projection.gtk4");
    (void)umi_appearance_copy_text(item->packet_id,sizeof item->packet_id,"packet.default");
    item->renderer=UMI_APPEARANCE_RENDERER_GTK4;
    item->semantic_revision=1U;
    item->projected_revision=1U;
    item->complete=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_renderer_theme_projection_is_valid(const UmiAppearanceRendererThemeProjection *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->projection_id) && umi_appearance_id_valid(item->packet_id) && item->projected_revision <= item->semantic_revision && item->projected_revision > 0U);
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/elevation_style_projection.c
 *
 * PURPOSE:
 *   Resolve semantic elevation levels to shadow and border tokens suitable for each frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/elevation_style_projection.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_elevation_style_projection_init(UmiAppearanceElevationStyleProjection *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->style_id,sizeof item->style_id,"elevation.panel");
    item->elevation_level=2;
    (void)umi_appearance_copy_text(item->shadow_token,sizeof item->shadow_token,"shadow.level2");
    (void)umi_appearance_copy_text(item->fallback_border_token,sizeof item->fallback_border_token,"color.border.strong");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_elevation_style_projection_is_valid(const UmiAppearanceElevationStyleProjection *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->style_id) && item->elevation_level >= 0);
}

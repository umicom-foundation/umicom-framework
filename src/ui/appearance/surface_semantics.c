/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/surface_semantics.c
 *
 * PURPOSE:
 *   Describe semantic surface hierarchy and elevation intent independently of renderer primitives.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/surface_semantics.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_surface_semantics_init(UmiAppearanceSurfaceSemantics *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->surface_id,sizeof item->surface_id,"surface.panel");
    (void)umi_appearance_copy_text(item->background_role,sizeof item->background_role,"surface.panel");
    (void)umi_appearance_copy_text(item->foreground_role,sizeof item->foreground_role,"text.primary");
    item->elevation_level=1;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_surface_semantics_is_valid(const UmiAppearanceSurfaceSemantics *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->surface_id) && item->elevation_level >= 0);
}

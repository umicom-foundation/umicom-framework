/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/high_dpi_policy.c
 *
 * PURPOSE:
 *   Define fractional-layout, snapping and asset-resolution rules for high-DPI displays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/high_dpi_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_high_dpi_policy_init(UmiAppearanceHighDpiPolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"dpi.production");
    item->allow_fractional_layout=true;
    item->snap_hairlines=true;
    item->prefer_vector_icons=true;
    item->maximum_raster_scale=4U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_high_dpi_policy_is_valid(const UmiAppearanceHighDpiPolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->maximum_raster_scale >= 1U);
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/scale_factor.c
 *
 * PURPOSE:
 *   Represent a bounded effective UI scale factor with independent OS and user contributions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/scale_factor.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_scale_factor_init(UmiAppearanceScaleFactor *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->scale_id,sizeof item->scale_id,"scale.default");
    item->os_factor=1.0;
    item->user_factor=1.0;
    item->effective_factor=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_scale_factor_is_valid(const UmiAppearanceScaleFactor *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->scale_id) && item->os_factor > 0.0 && item->user_factor > 0.0 && item->effective_factor > 0.0);
}

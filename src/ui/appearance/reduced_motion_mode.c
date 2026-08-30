/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/reduced_motion_mode.c
 *
 * PURPOSE:
 *   Resolve reduced-motion presentation requirements from user and system accessibility settings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/reduced_motion_mode.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_reduced_motion_mode_init(UmiAppearanceReducedMotionMode *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->mode_id,sizeof item->mode_id,"motion.reduced");
    item->enabled=true;
    item->maximum_duration_ms=80U;
    item->disable_decorative=true;
    item->preserve_essential_feedback=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_reduced_motion_mode_is_valid(const UmiAppearanceReducedMotionMode *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->mode_id) && item->maximum_duration_ms <= 500U);
}

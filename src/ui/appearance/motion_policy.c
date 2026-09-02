/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/motion_policy.c
 *
 * PURPOSE:
 *   Define semantic motion allowances and maximum transition durations for production UI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/motion_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_motion_policy_init(UmiAppearanceMotionPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"motion.default");
    item->standard_duration_ms=150U;
    item->emphasis_duration_ms=250U;
    item->allow_decorative_motion=true;
    item->allow_parallax=false;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_motion_policy_is_valid(const UmiAppearanceMotionPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->standard_duration_ms <= 2000U && item->emphasis_duration_ms <= 3000U);
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_metric_policy.c
 *
 * PURPOSE:
 *   Define renderer-neutral font metric tolerances used to prevent clipping and layout drift.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_metric_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_font_metric_policy_init(UmiAppearanceFontMetricPolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"metrics.ui");
    item->minimum_x_height_ratio=0.45;
    item->maximum_line_gap_ratio=0.50;
    item->baseline_tolerance_dp=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_font_metric_policy_is_valid(const UmiAppearanceFontMetricPolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->minimum_x_height_ratio > 0.0 && item->maximum_line_gap_ratio >= 0.0);
}

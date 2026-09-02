/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/font_metric_policy.h
 *
 * PURPOSE:
 *   Define renderer-neutral font metric tolerances used to prevent clipping and layout drift.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_FONT_METRIC_POLICY_H
#define UMICOM_UI_APPEARANCE_FONT_METRIC_POLICY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance font metric policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceFontMetricPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double minimum_x_height_ratio;
    double maximum_line_gap_ratio;
    double baseline_tolerance_dp;
} UmiAppearanceFontMetricPolicy;

/* Initialise one font metric policy record with deterministic defaults. */
UmiStatus umi_appearance_font_metric_policy_init(UmiAppearanceFontMetricPolicy *item);
/* Validate the required production invariants for this font metric policy. */
int umi_appearance_font_metric_policy_is_valid(const UmiAppearanceFontMetricPolicy *item);

#ifdef __cplusplus
}
#endif
#endif

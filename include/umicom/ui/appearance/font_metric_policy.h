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
#include "umicom/base/value_archive.h"
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_font_metric_policy_archive_encode(const UmiAppearanceFontMetricPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_font_metric_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceFontMetricPolicy *value);

#ifdef __cplusplus
}
#endif
#endif

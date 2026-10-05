/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/icon_scale_policy.h
 *
 * PURPOSE:
 *   Resolve logical icon size to physical pixels using the effective display scale.
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
#ifndef UMICOM_UI_APPEARANCE_ICON_SCALE_POLICY_H
#define UMICOM_UI_APPEARANCE_ICON_SCALE_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance icon scale policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceIconScalePolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double logical_size_dp;
    double scale;
    uint32_t physical_size_px;
} UmiAppearanceIconScalePolicy;

/* Initialise one icon scale policy record with deterministic defaults. */
UmiStatus umi_appearance_icon_scale_policy_init(UmiAppearanceIconScalePolicy *item);
/* Validate the required production invariants for this icon scale policy. */
int umi_appearance_icon_scale_policy_is_valid(const UmiAppearanceIconScalePolicy *item);
/* Round a positive scaled icon dimension to the nearest physical pixel. */
UmiStatus umi_appearance_icon_scale_policy_resolve(UmiAppearanceIconScalePolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_icon_scale_policy_archive_encode(const UmiAppearanceIconScalePolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_icon_scale_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceIconScalePolicy *value);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/text_scale_policy.h
 *
 * PURPOSE:
 *   Clamp user text scaling while preserving semantic size hierarchy and accessibility intent.
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
#ifndef UMICOM_UI_APPEARANCE_TEXT_SCALE_POLICY_H
#define UMICOM_UI_APPEARANCE_TEXT_SCALE_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance text scale policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceTextScalePolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double minimum_scale;
    double maximum_scale;
    double requested_scale;
    double resolved_scale;
} UmiAppearanceTextScalePolicy;

/* Initialise one text scale policy record with deterministic defaults. */
UmiStatus umi_appearance_text_scale_policy_init(UmiAppearanceTextScalePolicy *item);
/* Validate the required production invariants for this text scale policy. */
int umi_appearance_text_scale_policy_is_valid(const UmiAppearanceTextScalePolicy *item);
/* Resolve and clamp user text scaling through the shared appearance utility. */
UmiStatus umi_appearance_text_scale_policy_resolve(UmiAppearanceTextScalePolicy *item,double requested);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_text_scale_policy_archive_encode(const UmiAppearanceTextScalePolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_text_scale_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceTextScalePolicy *value);

#ifdef __cplusplus
}
#endif
#endif

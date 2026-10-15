/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/contrast_policy.h
 *
 * PURPOSE:
 *   Define certification thresholds for normal text, large text, icons and focus indicators.
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
#ifndef UMICOM_UI_APPEARANCE_CONTRAST_POLICY_H
#define UMICOM_UI_APPEARANCE_CONTRAST_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance contrast policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceContrastPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double normal_text_ratio;
    double large_text_ratio;
    double non_text_ratio;
    double focus_ratio;
} UmiAppearanceContrastPolicy;

/* Initialise one contrast policy record with deterministic defaults. */
UmiStatus umi_appearance_contrast_policy_init(UmiAppearanceContrastPolicy *item);
/* Validate the required production invariants for this contrast policy. */
int umi_appearance_contrast_policy_is_valid(const UmiAppearanceContrastPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_contrast_policy_archive_encode(const UmiAppearanceContrastPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_contrast_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceContrastPolicy *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_appearance_contrast_policy_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAppearanceContrastPolicy *expected, UmiAppearanceContrastPolicy *value);


#ifdef __cplusplus
}
#endif
#endif

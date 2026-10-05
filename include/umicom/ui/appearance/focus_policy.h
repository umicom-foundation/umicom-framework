/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/focus_policy.h
 *
 * PURPOSE:
 *   Define visible keyboard-focus treatment requirements across all renderer adapters.
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
#ifndef UMICOM_UI_APPEARANCE_FOCUS_POLICY_H
#define UMICOM_UI_APPEARANCE_FOCUS_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance focus policy data shared with callers of this public contract.
 */
typedef struct UmiAppearanceFocusPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    double ring_width;
    double ring_offset;
    bool always_visible_for_keyboard;
    bool clip_safe;
} UmiAppearanceFocusPolicy;

/* Initialise one focus policy record with deterministic defaults. */
UmiStatus umi_appearance_focus_policy_init(UmiAppearanceFocusPolicy *item);
/* Validate the required production invariants for this focus policy. */
int umi_appearance_focus_policy_is_valid(const UmiAppearanceFocusPolicy *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_focus_policy_archive_encode(const UmiAppearanceFocusPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_focus_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceFocusPolicy *value);

#ifdef __cplusplus
}
#endif
#endif

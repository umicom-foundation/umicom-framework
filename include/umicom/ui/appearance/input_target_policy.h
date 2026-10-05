/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/input_target_policy.h
 *
 * PURPOSE:
 *   Resolve minimum interactive target dimensions by pointer, touch, keyboard or hybrid modality.
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
#ifndef UMICOM_UI_APPEARANCE_INPUT_TARGET_POLICY_H
#define UMICOM_UI_APPEARANCE_INPUT_TARGET_POLICY_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance input target policy data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceInputTargetPolicy {
    char policy_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceInputModality modality;
    double minimum_width_dp;
    double minimum_height_dp;
} UmiAppearanceInputTargetPolicy;

/* Initialise one input target policy record with deterministic defaults. */
UmiStatus umi_appearance_input_target_policy_init(UmiAppearanceInputTargetPolicy *item);
/* Validate the required production invariants for this input target policy. */
int umi_appearance_input_target_policy_is_valid(const UmiAppearanceInputTargetPolicy *item);
/* Apply Framework baseline minimums for a requested input modality. */
UmiStatus umi_appearance_input_target_policy_for_modality(UmiAppearanceInputTargetPolicy *item,UmiAppearanceInputModality modality);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_input_target_policy_archive_encode(const UmiAppearanceInputTargetPolicy *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_input_target_policy_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceInputTargetPolicy *value);

#ifdef __cplusplus
}
#endif
#endif

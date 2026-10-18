/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/animation_gate.h
 *
 * PURPOSE:
 *   Decide whether an animation may run after reduced-motion and essential-feedback policy is applied.
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
#ifndef UMICOM_UI_APPEARANCE_ANIMATION_GATE_H
#define UMICOM_UI_APPEARANCE_ANIMATION_GATE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance animation gate data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAnimationGate {
    char animation_id[UMI_APPEARANCE_ID_CAPACITY];
    bool essential;
    bool reduced_motion;
    bool allowed;
} UmiAppearanceAnimationGate;

/* Initialise one animation gate record with deterministic defaults. */
UmiStatus umi_appearance_animation_gate_init(UmiAppearanceAnimationGate *item);
/* Validate the required production invariants for this animation gate. */
int umi_appearance_animation_gate_is_valid(const UmiAppearanceAnimationGate *item);
/* Recalculate animation permission from motion accessibility state. */
void umi_appearance_animation_gate_resolve(UmiAppearanceAnimationGate *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_animation_gate_archive_encode(const UmiAppearanceAnimationGate *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_animation_gate_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceAnimationGate *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAppearanceAnimationGate object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_appearance_animation_gate_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAppearanceAnimationGate *workspace, UmiAppearanceAnimationGate *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_appearance_animation_gate_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAppearanceAnimationGate *expected, UmiAppearanceAnimationGate *value);


#ifdef __cplusplus
}
#endif
#endif

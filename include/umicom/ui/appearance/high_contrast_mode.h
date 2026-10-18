/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/high_contrast_mode.h
 *
 * PURPOSE:
 *   Represent high-contrast presentation requirements layered over the canonical Design System.
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
#ifndef UMICOM_UI_APPEARANCE_HIGH_CONTRAST_MODE_H
#define UMICOM_UI_APPEARANCE_HIGH_CONTRAST_MODE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance high contrast mode data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceHighContrastMode {
    char mode_id[UMI_APPEARANCE_ID_CAPACITY];
    bool enabled;
    bool force_visible_borders;
    bool force_focus_outline;
    double minimum_border_width;
} UmiAppearanceHighContrastMode;

/* Initialise one high contrast mode record with deterministic defaults. */
UmiStatus umi_appearance_high_contrast_mode_init(UmiAppearanceHighContrastMode *item);
/* Validate the required production invariants for this high contrast mode. */
int umi_appearance_high_contrast_mode_is_valid(const UmiAppearanceHighContrastMode *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_high_contrast_mode_archive_encode(const UmiAppearanceHighContrastMode *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_high_contrast_mode_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceHighContrastMode *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiAppearanceHighContrastMode object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_appearance_high_contrast_mode_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiAppearanceHighContrastMode *workspace, UmiAppearanceHighContrastMode *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_appearance_high_contrast_mode_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiAppearanceHighContrastMode *expected, UmiAppearanceHighContrastMode *value);


#ifdef __cplusplus
}
#endif
#endif

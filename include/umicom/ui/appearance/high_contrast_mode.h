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

#ifdef __cplusplus
}
#endif
#endif

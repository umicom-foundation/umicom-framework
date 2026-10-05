/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/palette_override.h
 *
 * PURPOSE:
 *   Describe a scoped semantic palette override without embedding literal renderer colours.
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
#ifndef UMICOM_UI_APPEARANCE_PALETTE_OVERRIDE_H
#define UMICOM_UI_APPEARANCE_PALETTE_OVERRIDE_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance palette override data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearancePaletteOverride {
    char override_id[UMI_APPEARANCE_ID_CAPACITY];
    char role_id[UMI_APPEARANCE_ID_CAPACITY];
    char token_id[UMI_APPEARANCE_TOKEN_CAPACITY];
    UmiAppearanceScope scope;
} UmiAppearancePaletteOverride;

/* Initialise one palette override record with deterministic defaults. */
UmiStatus umi_appearance_palette_override_init(UmiAppearancePaletteOverride *item);
/* Validate the required production invariants for this palette override. */
int umi_appearance_palette_override_is_valid(const UmiAppearancePaletteOverride *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_palette_override_archive_encode(const UmiAppearancePaletteOverride *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_palette_override_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearancePaletteOverride *value);

#ifdef __cplusplus
}
#endif
#endif

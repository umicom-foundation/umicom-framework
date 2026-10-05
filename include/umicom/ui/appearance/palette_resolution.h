/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/palette_resolution.h
 *
 * PURPOSE:
 *   Record the winning token for a semantic palette role after scope precedence is applied.
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
#ifndef UMICOM_UI_APPEARANCE_PALETTE_RESOLUTION_H
#define UMICOM_UI_APPEARANCE_PALETTE_RESOLUTION_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance palette resolution data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearancePaletteResolution {
    char role_id[UMI_APPEARANCE_ID_CAPACITY];
    char base_token_id[UMI_APPEARANCE_TOKEN_CAPACITY];
    char resolved_token_id[UMI_APPEARANCE_TOKEN_CAPACITY];
    UmiAppearanceScope winning_scope;
} UmiAppearancePaletteResolution;

/* Initialise one palette resolution record with deterministic defaults. */
UmiStatus umi_appearance_palette_resolution_init(UmiAppearancePaletteResolution *item);
/* Validate the required production invariants for this palette resolution. */
int umi_appearance_palette_resolution_is_valid(const UmiAppearancePaletteResolution *item);
/* Apply an optional higher-precedence semantic token without copying toolkit colours. */
UmiStatus umi_appearance_palette_resolution_override(UmiAppearancePaletteResolution *item,const char *token_id,UmiAppearanceScope scope);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_palette_resolution_archive_encode(const UmiAppearancePaletteResolution *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_palette_resolution_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearancePaletteResolution *value);

#ifdef __cplusplus
}
#endif
#endif

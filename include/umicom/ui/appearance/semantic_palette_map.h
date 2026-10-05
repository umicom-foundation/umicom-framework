/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/semantic_palette_map.h
 *
 * PURPOSE:
 *   Map a semantic colour role to a Design-System token identity.
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
#ifndef UMICOM_UI_APPEARANCE_SEMANTIC_PALETTE_MAP_H
#define UMICOM_UI_APPEARANCE_SEMANTIC_PALETTE_MAP_H
#include "umicom/ui/appearance/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance semantic palette map data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceSemanticPaletteMap {
    char role_id[UMI_APPEARANCE_ID_CAPACITY];
    char token_id[UMI_APPEARANCE_TOKEN_CAPACITY];
} UmiAppearanceSemanticPaletteMap;

/* Initialise one semantic palette map record with deterministic defaults. */
UmiStatus umi_appearance_semantic_palette_map_init(UmiAppearanceSemanticPaletteMap *item);
/* Validate the required production invariants for this semantic palette map. */
int umi_appearance_semantic_palette_map_is_valid(const UmiAppearanceSemanticPaletteMap *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_appearance_semantic_palette_map_archive_encode(const UmiAppearanceSemanticPaletteMap *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_appearance_semantic_palette_map_archive_decode(const void *bytes, size_t byte_count,
    UmiAppearanceSemanticPaletteMap *value);

#ifdef __cplusplus
}
#endif
#endif

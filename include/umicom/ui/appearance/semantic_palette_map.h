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
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceSemanticPaletteMap {
    char role_id[UMI_APPEARANCE_ID_CAPACITY];
    char token_id[UMI_APPEARANCE_TOKEN_CAPACITY];
} UmiAppearanceSemanticPaletteMap;

/* Initialise one semantic palette map record with deterministic defaults. */
UmiStatus umi_appearance_semantic_palette_map_init(UmiAppearanceSemanticPaletteMap *item);
/* Validate the required production invariants for this semantic palette map. */
int umi_appearance_semantic_palette_map_is_valid(const UmiAppearanceSemanticPaletteMap *item);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_pack_registry.h
 *
 * PURPOSE:
 *   Catalogue versioned theme packs while preserving stable identity and revision evidence.
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
#ifndef UMICOM_UI_APPEARANCE_THEME_PACK_REGISTRY_H
#define UMICOM_UI_APPEARANCE_THEME_PACK_REGISTRY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/ui/appearance/theme_pack.h"

typedef struct UmiAppearanceThemePackRegistry { UmiAppearanceThemePack items[UMI_APPEARANCE_MAX_ITEMS]; size_t count; uint64_t revision; } UmiAppearanceThemePackRegistry;

/* Initialise an empty bounded registry. */
UmiStatus umi_appearance_theme_pack_registry_init(UmiAppearanceThemePackRegistry *registry);
/* Add or replace an item identified by its stable identifier. */
UmiStatus umi_appearance_theme_pack_registry_upsert(UmiAppearanceThemePackRegistry *registry, const UmiAppearanceThemePack *item);
/* Find an item by stable identifier. */
const UmiAppearanceThemePack *umi_appearance_theme_pack_registry_find(const UmiAppearanceThemePackRegistry *registry, const char *id);
/* Return the number of registered entries. */
size_t umi_appearance_theme_pack_registry_count(const UmiAppearanceThemePackRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

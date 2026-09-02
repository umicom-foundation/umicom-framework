/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/appearance_registry.h
 *
 * PURPOSE:
 *   Store bounded resolved appearance profiles for applications, workspaces and previews.
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
#ifndef UMICOM_UI_APPEARANCE_APPEARANCE_REGISTRY_H
#define UMICOM_UI_APPEARANCE_APPEARANCE_REGISTRY_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/ui/appearance/appearance_profile.h"

/**
 * Represent the appearance appearance registry data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceAppearanceRegistry { UmiAppearanceAppearanceProfile items[UMI_APPEARANCE_MAX_ITEMS]; size_t count; uint64_t revision; } UmiAppearanceAppearanceRegistry;

/* Initialise an empty bounded registry. */
UmiStatus umi_appearance_registry_init(UmiAppearanceAppearanceRegistry *registry);
/* Add or replace an item identified by its stable identifier. */
UmiStatus umi_appearance_registry_upsert(UmiAppearanceAppearanceRegistry *registry, const UmiAppearanceAppearanceProfile *item);
/* Find an item by stable identifier. */
const UmiAppearanceAppearanceProfile *umi_appearance_registry_find(const UmiAppearanceAppearanceRegistry *registry, const char *id);
/* Return the number of registered entries. */
size_t umi_appearance_registry_count(const UmiAppearanceAppearanceRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

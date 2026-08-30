/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/icon_catalogue.h
 *
 * PURPOSE:
 *   Catalogue reusable semantic icons independently of GTK, Qt or browser asset APIs.
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
#ifndef UMICOM_UI_APPEARANCE_ICON_CATALOGUE_H
#define UMICOM_UI_APPEARANCE_ICON_CATALOGUE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
#include "umicom/ui/appearance/icon_descriptor.h"

typedef struct UmiAppearanceIconCatalogue { UmiAppearanceIconDescriptor items[UMI_APPEARANCE_MAX_ICONS]; size_t count; uint64_t revision; } UmiAppearanceIconCatalogue;

/* Initialise an empty bounded registry. */
UmiStatus umi_appearance_icon_catalogue_init(UmiAppearanceIconCatalogue *registry);
/* Add or replace an item identified by its stable identifier. */
UmiStatus umi_appearance_icon_catalogue_upsert(UmiAppearanceIconCatalogue *registry, const UmiAppearanceIconDescriptor *item);
/* Find an item by stable identifier. */
const UmiAppearanceIconDescriptor *umi_appearance_icon_catalogue_find(const UmiAppearanceIconCatalogue *registry, const char *id);
/* Return the number of registered entries. */
size_t umi_appearance_icon_catalogue_count(const UmiAppearanceIconCatalogue *registry);

#ifdef __cplusplus
}
#endif
#endif

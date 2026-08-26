/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/icon_variant_resolution.h
 *
 * PURPOSE:
 *   Resolve light/dark/high-contrast and direction-aware icon variants while preserving semantic identity.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_ICON_VARIANT_RESOLUTION_H
#define UMICOM_UI_APPEARANCE_ICON_VARIANT_RESOLUTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceIconVariantResolution {
    char icon_id[UMI_APPEARANCE_ID_CAPACITY];
    char resolved_variant_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignThemeMode mode;
    bool rtl;
    bool mirrored;
} UmiAppearanceIconVariantResolution;

/* Initialise one icon variant resolution record with deterministic defaults. */
UmiStatus umi_appearance_icon_variant_resolution_init(UmiAppearanceIconVariantResolution *item);
/* Validate the required production invariants for this icon variant resolution. */
int umi_appearance_icon_variant_resolution_is_valid(const UmiAppearanceIconVariantResolution *item);
/* Resolve direction-sensitive mirroring without changing the canonical icon identity. */
void umi_appearance_icon_variant_resolution_set_direction(UmiAppearanceIconVariantResolution *item,int rtl,int direction_sensitive);

#ifdef __cplusplus
}
#endif
#endif

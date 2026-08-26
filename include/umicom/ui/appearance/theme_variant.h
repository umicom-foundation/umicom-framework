/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_variant.h
 *
 * PURPOSE:
 *   Bind a semantic theme pack to light, dark or high-contrast presentation mode.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_THEME_VARIANT_H
#define UMICOM_UI_APPEARANCE_THEME_VARIANT_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceThemeVariant {
    char variant_id[UMI_APPEARANCE_ID_CAPACITY];
    char pack_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignThemeMode mode;
    bool preferred;
} UmiAppearanceThemeVariant;

/* Initialise one theme variant record with deterministic defaults. */
UmiStatus umi_appearance_theme_variant_init(UmiAppearanceThemeVariant *item);
/* Validate the required production invariants for this theme variant. */
int umi_appearance_theme_variant_is_valid(const UmiAppearanceThemeVariant *item);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/theme_pack.h
 *
 * PURPOSE:
 *   Describe a versionable semantic theme pack without toolkit CSS or widget classes.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_THEME_PACK_H
#define UMICOM_UI_APPEARANCE_THEME_PACK_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceThemePack {
    char pack_id[UMI_APPEARANCE_ID_CAPACITY];
    char parent_pack_id[UMI_APPEARANCE_ID_CAPACITY];
    char brand_id[UMI_APPEARANCE_ID_CAPACITY];
    char token_set_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiDesignThemeMode mode;
    uint32_t revision;
} UmiAppearanceThemePack;

/* Initialise one theme pack record with deterministic defaults. */
UmiStatus umi_appearance_theme_pack_init(UmiAppearanceThemePack *item);
/* Validate the required production invariants for this theme pack. */
int umi_appearance_theme_pack_is_valid(const UmiAppearanceThemePack *item);

#ifdef __cplusplus
}
#endif
#endif

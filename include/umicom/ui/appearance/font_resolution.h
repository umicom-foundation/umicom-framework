/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/font_resolution.h
 *
 * PURPOSE:
 *   Record the winning family and fallback depth selected for a semantic font stack.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_FONT_RESOLUTION_H
#define UMICOM_UI_APPEARANCE_FONT_RESOLUTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceFontResolution {
    char stack_id[UMI_APPEARANCE_ID_CAPACITY];
    char resolved_family_id[UMI_APPEARANCE_ID_CAPACITY];
    uint32_t fallback_depth;
    bool available;
} UmiAppearanceFontResolution;

/* Initialise one font resolution record with deterministic defaults. */
UmiStatus umi_appearance_font_resolution_init(UmiAppearanceFontResolution *item);
/* Validate the required production invariants for this font resolution. */
int umi_appearance_font_resolution_is_valid(const UmiAppearanceFontResolution *item);
/* Resolve to the preferred available family, falling back when the preferred identity is empty. */
UmiStatus umi_appearance_font_resolution_choose(UmiAppearanceFontResolution *item,const char *preferred,const char *fallback);

#ifdef __cplusplus
}
#endif
#endif

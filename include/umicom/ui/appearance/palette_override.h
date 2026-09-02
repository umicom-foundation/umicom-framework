/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/palette_override.h
 *
 * PURPOSE:
 *   Describe a scoped semantic palette override without embedding literal renderer colours.
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
#ifndef UMICOM_UI_APPEARANCE_PALETTE_OVERRIDE_H
#define UMICOM_UI_APPEARANCE_PALETTE_OVERRIDE_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance palette override data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearancePaletteOverride {
    char override_id[UMI_APPEARANCE_ID_CAPACITY];
    char role_id[UMI_APPEARANCE_ID_CAPACITY];
    char token_id[UMI_APPEARANCE_TOKEN_CAPACITY];
    UmiAppearanceScope scope;
} UmiAppearancePaletteOverride;

/* Initialise one palette override record with deterministic defaults. */
UmiStatus umi_appearance_palette_override_init(UmiAppearancePaletteOverride *item);
/* Validate the required production invariants for this palette override. */
int umi_appearance_palette_override_is_valid(const UmiAppearancePaletteOverride *item);

#ifdef __cplusplus
}
#endif
#endif

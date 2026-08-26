/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/color_palette.h
 *
 * PURPOSE:
 *   Store bounded semantic colour-role mappings that can be shared by every application frontend.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_UI_DESIGN_COLOR_PALETTE_H
#define INCLUDE_UMICOM_UI_DESIGN_COLOR_PALETTE_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/color_role.h"
#include "umicom/ui/design/color.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/color_role.h"
#include "umicom/ui/design/color.h"

typedef struct UmiDesignColorPaletteEntry { UmiDesignColorRole role; UmiDesignRgba color; } UmiDesignColorPaletteEntry;
typedef struct UmiDesignColorPalette { UmiDesignColorPaletteEntry entries[UMI_DESIGN_MAX_ITEMS]; size_t count; } UmiDesignColorPalette;

/* Insert or replace one semantic colour role in the bounded palette. */
UmiStatus umi_design_color_palette_upsert(UmiDesignColorPalette *palette, UmiDesignColorRole role, UmiDesignRgba color);
/* Resolve one semantic role from the palette. */
UmiStatus umi_design_color_palette_find(const UmiDesignColorPalette *palette, UmiDesignColorRole role, UmiDesignRgba *out_color);

#ifdef __cplusplus
}
#endif

#endif

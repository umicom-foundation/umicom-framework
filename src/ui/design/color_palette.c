/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/color_palette.c
 *
 * PURPOSE:
 *   Store bounded semantic colour-role mappings that can be shared by every application frontend.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/color_palette.h"

UmiStatus umi_design_color_palette_upsert(UmiDesignColorPalette *palette,UmiDesignColorRole role,UmiDesignRgba color)
{
    size_t i;
    if (palette==NULL || !umi_design_color_valid(color) || role<UMI_DESIGN_COLOR_BACKGROUND || role>UMI_DESIGN_COLOR_BORDER) return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<palette->count;++i) if(palette->entries[i].role==role){ palette->entries[i].color=color; return UMI_STATUS_OK; }
    if(palette->count>=UMI_DESIGN_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
    palette->entries[palette->count].role=role; palette->entries[palette->count].color=color; ++palette->count; return UMI_STATUS_OK;
}
UmiStatus umi_design_color_palette_find(const UmiDesignColorPalette *palette,UmiDesignColorRole role,UmiDesignRgba *out_color)
{
    size_t i; if(palette==NULL||out_color==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<palette->count;++i) if(palette->entries[i].role==role){*out_color=palette->entries[i].color;return UMI_STATUS_OK;}
    return UMI_STATUS_NOT_FOUND;
}

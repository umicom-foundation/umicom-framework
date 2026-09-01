/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/brand_palette.c
 *
 * PURPOSE:
 *   Store the canonical Umicom colour variants in one toolkit-neutral module
 *   so every application uses the same readable surfaces and brand accents.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/brand_palette.h"

#include <stddef.h>

/* The dark palette separates the application background, tool surfaces and
 * editor without using highly saturated areas behind the wordmark. */
static const UmiUiBrandPalette DARK_PALETTE = {
    "#0B1220", "#121C2A", "#1A2636", "#223247", "#0E1724",
    "#2B3A4D", "#41556D", "#F4F7FB", "#A9B6C6", "#4C8ED9",
    "#1D3855", "#43B581", "#E4B04A", "#E56874", "#5B91B8",
    "#D04B55", "#F4F7FB"
};

/* The light palette keeps the same semantic hierarchy with darker foreground
 * values and a deeper blue accent that remains visible on white surfaces. */
static const UmiUiBrandPalette LIGHT_PALETTE = {
    "#F4F7FA", "#FFFFFF", "#EAF0F6", "#DEE8F2", "#FFFFFF",
    "#C8D3DF", "#9AAABD", "#152235", "#586A7E", "#1E5D93",
    "#DDECF8", "#247A52", "#8A5B0A", "#B32D3A", "#315B78",
    "#B4363F", "#203247"
};

/* High contrast uses direct black, white and bright state colours so the
 * interface stays understandable when subtle surface differences disappear. */
static const UmiUiBrandPalette HIGH_CONTRAST_PALETTE = {
    "#000000", "#000000", "#111111", "#222222", "#000000",
    "#FFFFFF", "#FFFFFF", "#FFFFFF", "#E0E0E0", "#FFFF00",
    "#303000", "#00FF80", "#FFFF00", "#FF5F5F", "#FFFFFF",
    "#FFFF00", "#FFFFFF"
};

/* A semantic colour is present only when it points to a non-empty string. */
static int palette_colour_is_present(const char *colour)
{
    return colour != NULL && colour[0] != '\0';
}

const UmiUiBrandPalette *umi_ui_brand_palette_get(
    UmiUiBrandSurface surface)
{
    /* Each supported variant maps to one immutable object, so applications
     * can safely retain the returned non-owning pointer. */
    switch (surface) {
        case UMI_UI_BRAND_SURFACE_LIGHT:
            return &LIGHT_PALETTE;
        case UMI_UI_BRAND_SURFACE_DARK:
            return &DARK_PALETTE;
        case UMI_UI_BRAND_SURFACE_HIGH_CONTRAST:
            return &HIGH_CONTRAST_PALETTE;
        default:
            return NULL;
    }
}

UmiStatus umi_ui_brand_palette_validate(
    const UmiUiBrandPalette *palette)
{
    /* Checking all fields here prevents an adapter from discovering a missing
     * colour halfway through composing an application window. */
    if (palette == NULL ||
        !palette_colour_is_present(palette->background) ||
        !palette_colour_is_present(palette->surface) ||
        !palette_colour_is_present(palette->surface_raised) ||
        !palette_colour_is_present(palette->surface_hover) ||
        !palette_colour_is_present(palette->editor) ||
        !palette_colour_is_present(palette->border) ||
        !palette_colour_is_present(palette->border_strong) ||
        !palette_colour_is_present(palette->foreground) ||
        !palette_colour_is_present(palette->foreground_muted) ||
        !palette_colour_is_present(palette->accent) ||
        !palette_colour_is_present(palette->accent_surface) ||
        !palette_colour_is_present(palette->success) ||
        !palette_colour_is_present(palette->warning) ||
        !palette_colour_is_present(palette->danger) ||
        !palette_colour_is_present(palette->mark_blue) ||
        !palette_colour_is_present(palette->mark_red) ||
        !palette_colour_is_present(palette->logo_foreground)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

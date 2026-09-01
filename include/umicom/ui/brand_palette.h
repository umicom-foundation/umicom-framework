/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/brand_palette.h
 *
 * PURPOSE:
 *   Publish the shared semantic colour palette used by every Umicom
 *   application. Applications ask for a surface variant instead of copying
 *   colour values into their own source code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_BRAND_PALETTE_H
#define UMICOM_UI_BRAND_PALETTE_H

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* A surface variant describes the brightness and contrast expected behind
 * text, icons and controls. It does not name any particular UI toolkit. */
typedef enum UmiUiBrandSurface {
    UMI_UI_BRAND_SURFACE_LIGHT = 1,
    UMI_UI_BRAND_SURFACE_DARK = 2,
    UMI_UI_BRAND_SURFACE_HIGH_CONTRAST = 3
} UmiUiBrandSurface;

/* Each field has a job rather than a widget name. This lets a desktop, web or
 * terminal adapter use the same meaning with different native controls. */
typedef struct UmiUiBrandPalette {
    const char *background;
    const char *surface;
    const char *surface_raised;
    const char *surface_hover;
    const char *editor;
    const char *border;
    const char *border_strong;
    const char *foreground;
    const char *foreground_muted;
    const char *accent;
    const char *accent_surface;
    const char *success;
    const char *warning;
    const char *danger;
    const char *mark_blue;
    const char *mark_red;
    const char *logo_foreground;
} UmiUiBrandPalette;

/* Return an immutable palette owned by Framework. The pointer remains valid
 * for the lifetime of the process and must never be freed by the caller. */
const UmiUiBrandPalette *umi_ui_brand_palette_get(
    UmiUiBrandSurface surface);

/* Validate that a palette contains every semantic colour required by an
 * application shell. This is useful in tests and adapter startup checks. */
UmiStatus umi_ui_brand_palette_validate(
    const UmiUiBrandPalette *palette);

#ifdef __cplusplus
}
#endif

#endif

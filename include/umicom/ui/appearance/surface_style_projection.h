/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/surface_style_projection.h
 *
 * PURPOSE:
 *   Resolve semantic surface roles to token identities consumed by frontend renderers.
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
#ifndef UMICOM_UI_APPEARANCE_SURFACE_STYLE_PROJECTION_H
#define UMICOM_UI_APPEARANCE_SURFACE_STYLE_PROJECTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceSurfaceStyleProjection {
    char surface_id[UMI_APPEARANCE_ID_CAPACITY];
    char background_token[UMI_APPEARANCE_TOKEN_CAPACITY];
    char foreground_token[UMI_APPEARANCE_TOKEN_CAPACITY];
    char border_token[UMI_APPEARANCE_TOKEN_CAPACITY];
} UmiAppearanceSurfaceStyleProjection;

/* Initialise one surface style projection record with deterministic defaults. */
UmiStatus umi_appearance_surface_style_projection_init(UmiAppearanceSurfaceStyleProjection *item);
/* Validate the required production invariants for this surface style projection. */
int umi_appearance_surface_style_projection_is_valid(const UmiAppearanceSurfaceStyleProjection *item);

#ifdef __cplusplus
}
#endif
#endif

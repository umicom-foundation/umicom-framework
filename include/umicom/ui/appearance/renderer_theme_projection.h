/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/renderer_theme_projection.h
 *
 * PURPOSE:
 *   Record one renderer-specific projection of a semantic style packet without transferring state ownership.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_RENDERER_THEME_PROJECTION_H
#define UMICOM_UI_APPEARANCE_RENDERER_THEME_PROJECTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceRendererThemeProjection {
    char projection_id[UMI_APPEARANCE_ID_CAPACITY];
    char packet_id[UMI_APPEARANCE_ID_CAPACITY];
    UmiAppearanceRendererKind renderer;
    uint64_t semantic_revision;
    uint64_t projected_revision;
    bool complete;
} UmiAppearanceRendererThemeProjection;

/* Initialise one renderer theme projection record with deterministic defaults. */
UmiStatus umi_appearance_renderer_theme_projection_init(UmiAppearanceRendererThemeProjection *item);
/* Validate the required production invariants for this renderer theme projection. */
int umi_appearance_renderer_theme_projection_is_valid(const UmiAppearanceRendererThemeProjection *item);

#ifdef __cplusplus
}
#endif
#endif

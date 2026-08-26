/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/elevation_style_projection.h
 *
 * PURPOSE:
 *   Resolve semantic elevation levels to shadow and border tokens suitable for each frontend.
 *
 * ARCHITECTURE:
 *   This production appearance capability extends canonical Umicom::ui and
 *   composes the existing Design System, adaptive shell and renderer contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_APPEARANCE_ELEVATION_STYLE_PROJECTION_H
#define UMICOM_UI_APPEARANCE_ELEVATION_STYLE_PROJECTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceElevationStyleProjection {
    char style_id[UMI_APPEARANCE_ID_CAPACITY];
    int32_t elevation_level;
    char shadow_token[UMI_APPEARANCE_TOKEN_CAPACITY];
    char fallback_border_token[UMI_APPEARANCE_TOKEN_CAPACITY];
} UmiAppearanceElevationStyleProjection;

/* Initialise one elevation style projection record with deterministic defaults. */
UmiStatus umi_appearance_elevation_style_projection_init(UmiAppearanceElevationStyleProjection *item);
/* Validate the required production invariants for this elevation style projection. */
int umi_appearance_elevation_style_projection_is_valid(const UmiAppearanceElevationStyleProjection *item);

#ifdef __cplusplus
}
#endif
#endif

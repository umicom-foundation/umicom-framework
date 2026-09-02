/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/border_style_projection.h
 *
 * PURPOSE:
 *   Resolve semantic border width, radius and token identity without exposing toolkit CSS syntax.
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
#ifndef UMICOM_UI_APPEARANCE_BORDER_STYLE_PROJECTION_H
#define UMICOM_UI_APPEARANCE_BORDER_STYLE_PROJECTION_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance border style projection data shared with callers of this public
 * contract.
 */
typedef struct UmiAppearanceBorderStyleProjection {
    char style_id[UMI_APPEARANCE_ID_CAPACITY];
    char color_token[UMI_APPEARANCE_TOKEN_CAPACITY];
    double width_dp;
    double radius_dp;
    bool visible;
} UmiAppearanceBorderStyleProjection;

/* Initialise one border style projection record with deterministic defaults. */
UmiStatus umi_appearance_border_style_projection_init(UmiAppearanceBorderStyleProjection *item);
/* Validate the required production invariants for this border style projection. */
int umi_appearance_border_style_projection_is_valid(const UmiAppearanceBorderStyleProjection *item);

#ifdef __cplusplus
}
#endif
#endif

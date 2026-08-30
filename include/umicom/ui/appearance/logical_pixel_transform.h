/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/logical_pixel_transform.h
 *
 * PURPOSE:
 *   Convert physical pixel measurements to renderer-neutral logical coordinates.
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
#ifndef UMICOM_UI_APPEARANCE_LOGICAL_PIXEL_TRANSFORM_H
#define UMICOM_UI_APPEARANCE_LOGICAL_PIXEL_TRANSFORM_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiAppearanceLogicalPixelTransform { double scale; } UmiAppearanceLogicalPixelTransform;
/* Initialise one pixel transform with a positive scale factor. */
UmiStatus umi_appearance_logical_pixel_transform_init(UmiAppearanceLogicalPixelTransform *item,double scale);
/* Convert one measurement through the resolved display scale. */
UmiStatus umi_appearance_logical_pixel_transform_convert(const UmiAppearanceLogicalPixelTransform *item,double input,double *output);

#ifdef __cplusplus
}
#endif
#endif

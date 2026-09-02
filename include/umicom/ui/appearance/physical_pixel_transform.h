/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/appearance/physical_pixel_transform.h
 *
 * PURPOSE:
 *   Convert logical design measurements to physical device pixels.
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
#ifndef UMICOM_UI_APPEARANCE_PHYSICAL_PIXEL_TRANSFORM_H
#define UMICOM_UI_APPEARANCE_PHYSICAL_PIXEL_TRANSFORM_H
#include "umicom/ui/appearance/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the appearance physical pixel transform data shared with callers of this
 * public contract.
 */
typedef struct UmiAppearancePhysicalPixelTransform { double scale; } UmiAppearancePhysicalPixelTransform;
/* Initialise one pixel transform with a positive scale factor. */
UmiStatus umi_appearance_physical_pixel_transform_init(UmiAppearancePhysicalPixelTransform *item,double scale);
/* Convert one measurement through the resolved display scale. */
UmiStatus umi_appearance_physical_pixel_transform_convert(const UmiAppearancePhysicalPixelTransform *item,double input,double *output);

#ifdef __cplusplus
}
#endif
#endif

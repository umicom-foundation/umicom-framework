/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media/image_composite.h
 *
 * PURPOSE:
 *   Compose straight-alpha artwork through the owned image service without modifying either input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_IMAGE_COMPOSITE_H
#define UMICOM_MEDIA_IMAGE_COMPOSITE_H
#include "umicom/media/image_edit.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Compose straight-alpha RGBA source-over into a separate owned surface.
 * The complete overlay must fit at (x,y); no clipping, resizing or file access
 * occurs. Both inputs remain unchanged and may be the same surface. Keep them
 * stable throughout the call. The result uses the source dimensions and is
 * limited to UMI_MEDIA_IMAGE_EDIT_MAX_PIXELS.
 * Initialise *out_surface=NULL. All refusals and cancellation preserve it.
 * Colors are blended in their stored sample space, without ICC/gamma conversion;
 * integer rounding is nearest. A fully transparent overlay preserves original
 * bytes, including hidden color channels. Cancel is checked between rows and
 * immediately before publication. Destroy the result with the normal owner. */
UmiStatus UmiMediaImageSurfaceComposite(const UmiMediaImageSurface *source,
    const UmiMediaImageSurface *overlay, size_t x, size_t y,
    const UmiCancellationToken *cancel, UmiMediaImageSurface **out_surface);
#ifdef __cplusplus
}
#endif
#endif

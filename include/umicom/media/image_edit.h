/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media/image_edit.h
 * PURPOSE: Describe reversible crop and orientation operations that create a separate owned Media image.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_MEDIA_IMAGE_EDIT_H
#define UMICOM_MEDIA_IMAGE_EDIT_H
#include "umicom/media/image_surface.h"
#include "umicom/platform/cancellation.h"
#ifdef __cplusplus
extern "C"
{
#endif
#define UMI_MEDIA_IMAGE_EDIT_MAX_PIXELS (16U * 1024U * 1024U)
    typedef enum UmiMediaImageOrientation
    {
        UMI_MEDIA_IMAGE_ORIGINAL = 0,
        UMI_MEDIA_IMAGE_CLOCKWISE,
        UMI_MEDIA_IMAGE_HALF_TURN,
        UMI_MEDIA_IMAGE_COUNTERCLOCKWISE,
        UMI_MEDIA_IMAGE_FLIP_HORIZONTAL,
        UMI_MEDIA_IMAGE_FLIP_VERTICAL
    } UmiMediaImageOrientation;
    typedef struct UmiMediaImageEdit
    {
        size_t x, y, width, height;           /* Nonempty crop in original pixel coordinates. */
        UmiMediaImageOrientation orientation; /* Applied after cropping. */
    } UmiMediaImageEdit;
    /* Initialise *out_surface=NULL. Validate the complete crop/orientation before
 * allocation, then copy exact RGBA pixels to a new surface, at most MAX_PIXELS.
 * The source is unchanged; failure/cancellation never publishes a partial image.
 * Clockwise/counterclockwise mean quarter turns. No interpolation, colour
 * conversion, file I/O or implicit metadata transformation occurs. Keep the
 * source stable for the full call, and destroy the result with the normal
 * Media image owner. Cancellation is checked between destination rows. */
    UmiStatus UmiMediaImageSurfaceApplyEdit(const UmiMediaImageSurface *source, const UmiMediaImageEdit *edit,
                                            const UmiCancellationToken *cancel,
                                            UmiMediaImageSurface **out_surface);
#ifdef __cplusplus
}
#endif
#endif

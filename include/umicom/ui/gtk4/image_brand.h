/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/image_brand.h
 *
 * PURPOSE:
 *   Expose canonical SVG composition and exclusive branded PNG export through the native Framework adapter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_IMAGE_BRAND_H
#define UMICOM_UI_GTK4_IMAGE_BRAND_H
#include "umicom/media/png_image.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Decode embedded canonical SVG and compose a separate branded image.
 * No GTK widget/display, network or file access is required. Call on a worker
 * and keep source stable. SVG loader support must be installed; failure never
 * returns an unbranded substitute. Source dimensions must be at least 128x64
 * and within the shared PNG limits. The wordmark fits inside one fifth of each
 * dimension, with a dark backing and two-percent margins. Source is unchanged.
 * Initialise *out_surface=NULL; failure/cancellation preserve it. */
UmiStatus UmiGtk4ImageApplyBrand(const UmiMediaImageSurface *source,
    const UmiCancellationToken *cancel, UmiMediaImageSurface **out_surface);
/** Compose then use the existing exclusive PNG writer. The receipt is cleared
 * before admission and records actual output creation/bytes. Composition failure
 * creates no output; a later I/O failure can leave a partial new PNG. */
UmiStatus UmiGtk4ImageWriteBrandedPng(const UmiMediaImageSurface *source, const char *path,
    const UmiCancellationToken *cancel, UmiMediaPngWriteResult *out);
#ifdef __cplusplus
}
#endif
#endif

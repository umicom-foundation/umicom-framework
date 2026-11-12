/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/image_brand_gtk4.c
 *
 * PURPOSE:
 *   Reuse canonical SVG decoding and immutable Media composition on a worker before PNG publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/image_brand.h"
#include "umicom/media/image_composite.h"
#include "brand_widget.h"
#include <string.h>

UmiStatus UmiGtk4ImageApplyBrand(const UmiMediaImageSurface *source,
    const UmiCancellationToken *cancel, UmiMediaImageSurface **out_surface)
{
    if (source == NULL || out_surface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_surface != NULL) return UMI_STATUS_INVALID_STATE;
    UmiMediaImageSurfaceSnapshot info;
    UmiStatus status = umi_media_image_surface_snapshot(source, &info);
    if (status != UMI_STATUS_OK) return status;
    if (info.width < 128U || info.height < 64U) return UMI_STATUS_INVALID_ARGUMENT;
    if (info.width > UMI_MEDIA_PNG_MAX_DIMENSION || info.height > UMI_MEDIA_PNG_MAX_DIMENSION ||
        info.pixel_count > UMI_MEDIA_PNG_MAX_PIXELS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    /* Reuse the native canonical decoder: it reads embedded SVG path artwork,
     * not a font-dependent reconstruction or a current-directory bitmap. */
    size_t height = MIN(info.height / 5U, info.width * 261U / (1064U * 5U));
    height = MIN(height, 256U);
    GError *error = NULL;
    GdkPixbuf *decoded = UmiGtkBrandDecode(TRUE, TRUE, (int)height, 1, &error);
    if (decoded == NULL) {
        g_clear_error(&error);
        return UMI_STATUS_IO_ERROR;
    }
    int width = gdk_pixbuf_get_width(decoded), rows = gdk_pixbuf_get_height(decoded);
    int channels = gdk_pixbuf_get_n_channels(decoded), stride = gdk_pixbuf_get_rowstride(decoded);
    const guchar *pixels = gdk_pixbuf_read_pixels(decoded);
    if (width <= 0 || rows <= 0 || width > 1064 || rows > 256 || pixels == NULL ||
        (channels != 3 && channels != 4) || stride < width * channels ||
        gdk_pixbuf_get_bits_per_sample(decoded) != 8 ||
        gdk_pixbuf_get_colorspace(decoded) != GDK_COLORSPACE_RGB) {
        g_object_unref(decoded);
        return UMI_STATUS_INVALID_STATE;
    }
    UmiMediaImageSurface *logo = NULL, *backing = NULL, *framed = NULL;
    status = umi_media_image_surface_create((size_t)width, (size_t)rows, &logo);
    for (int row = 0; status == UMI_STATUS_OK && row < rows; ++row) {
        if (umi_cancellation_token_is_requested(cancel)) { status = UMI_STATUS_CANCELLED; break; }
        const guchar *input = pixels + (size_t)row * (size_t)stride;
        for (int column = 0; status == UMI_STATUS_OK && column < width; ++column) {
            const guchar *p = input + (size_t)column * (size_t)channels;
            UmiMediaRgbaPixel pixel = {p[0], p[1], p[2], (uint8_t)(channels == 4 ? p[3] : 255U)};
            status = umi_media_image_surface_set_pixel(logo, (size_t)column, (size_t)row, pixel);
        }
    }
    g_object_unref(decoded);
    size_t framedWidth = (size_t)width + 4U, framedHeight = (size_t)rows + 4U;
    if (status == UMI_STATUS_OK)
        status = umi_media_image_surface_create(framedWidth, framedHeight, &backing);
    if (status == UMI_STATUS_OK)
        status = umi_media_image_surface_clear(backing, (UmiMediaRgbaPixel){13U, 24U, 38U, 192U});
    if (status == UMI_STATUS_OK)
        status = UmiMediaImageSurfaceComposite(backing, logo, 2U, 2U, cancel, &framed);
    size_t marginX = MAX(1U, info.width / 50U), marginY = MAX(1U, info.height / 50U);
    if (status == UMI_STATUS_OK &&
        (framedWidth > info.width - marginX || framedHeight > info.height - marginY))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        status = UmiMediaImageSurfaceComposite(source, framed,
            info.width - marginX - framedWidth, info.height - marginY - framedHeight,
            cancel, out_surface);
    umi_media_image_surface_destroy(framed);
    umi_media_image_surface_destroy(backing);
    umi_media_image_surface_destroy(logo);
    return status;
}

UmiStatus UmiGtk4ImageWriteBrandedPng(const UmiMediaImageSurface *source, const char *path,
    const UmiCancellationToken *cancel, UmiMediaPngWriteResult *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    UmiStatus status = UmiOutputFileValidatePath(path);
    if (status != UMI_STATUS_OK) return status;
    UmiMediaImageSurface *branded = NULL;
    status = UmiGtk4ImageApplyBrand(source, cancel, &branded);
    if (status == UMI_STATUS_OK)
        status = UmiMediaPngWriteNew(branded, path, cancel, out);
    umi_media_image_surface_destroy(branded);
    return status;
}

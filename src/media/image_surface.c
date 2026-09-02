/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media/image_surface.c
 *
 * PURPOSE:
 *   Implement an overflow-checked owned image surface with copied pixel access.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/media/image_surface.h"

#include <stdlib.h>
#include <string.h>

/* Private storage keeps pixel lifetime tied to one surface owner. */
struct UmiMediaImageSurface {
    size_t width;
    size_t height;
    size_t pixel_count;
    UmiMediaRgbaPixel *pixels;
    uint64_t revision;
};

/* Convert checked coordinates to a flat storage index. */
static UmiStatus pixel_index(
    const UmiMediaImageSurface *surface,
    size_t x,
    size_t y,
    size_t *out_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Coordinates at or beyond a dimension are outside the owned allocation. */
    if (x >= surface->width || y >= surface->height) {
        return UMI_STATUS_NOT_FOUND;
    }
    *out_index = y * surface->width + x;
    return UMI_STATUS_OK;
}

/* Allocate a pixel buffer only after checking both multiplication operations. */
UmiStatus umi_media_image_surface_create(
    size_t width,
    size_t height,
    UmiMediaImageSurface **out_surface)
{
    UmiMediaImageSurface *surface;
    size_t pixel_count;
    /* Zero dimensions and width-by-height overflow cannot describe an image. */
    if (out_surface == NULL || width == 0U || height == 0U ||
        height > SIZE_MAX / width) return UMI_STATUS_INVALID_ARGUMENT;
    pixel_count = width * height;
    /* Pixel-count multiplication is checked independently from dimensions. */
    if (pixel_count > SIZE_MAX / sizeof(UmiMediaRgbaPixel)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_surface = NULL;
    surface = (UmiMediaImageSurface *)calloc(1U, sizeof(*surface));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    surface->pixels = (UmiMediaRgbaPixel *)calloc(
        pixel_count, sizeof(*surface->pixels));
    /* Release the surface owner when the pixel allocation cannot be completed. */
    if (surface->pixels == NULL) {
        free(surface);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    surface->width = width;
    surface->height = height;
    surface->pixel_count = pixel_count;
    surface->revision = 1U;
    *out_surface = surface;
    return UMI_STATUS_OK;
}

/* Release pixels before their surface owner; NULL destruction is safe. */
void umi_media_image_surface_destroy(UmiMediaImageSurface *surface)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return;
    free(surface->pixels);
    free(surface);
}

/* Fill all pixels explicitly because RGBA channels may not share one byte value. */
UmiStatus umi_media_image_surface_clear(
    UmiMediaImageSurface *surface,
    UmiMediaRgbaPixel pixel)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < surface->pixel_count; ++index) {
        surface->pixels[index] = pixel;
    }
    surface->revision += 1U;
    return UMI_STATUS_OK;
}

/* Replace one pixel only after the common coordinate boundary check succeeds. */
UmiStatus umi_media_image_surface_set_pixel(
    UmiMediaImageSurface *surface,
    size_t x,
    size_t y,
    UmiMediaRgbaPixel pixel)
{
    size_t index;
    UmiStatus status = pixel_index(surface, x, y, &index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    surface->pixels[index] = pixel;
    surface->revision += 1U;
    return UMI_STATUS_OK;
}

/* Copy one pixel to prevent callers from retaining writable buffer pointers. */
UmiStatus umi_media_image_surface_get_pixel(
    const UmiMediaImageSurface *surface,
    size_t x,
    size_t y,
    UmiMediaRgbaPixel *out_pixel)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_pixel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = pixel_index(surface, x, y, &index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    *out_pixel = surface->pixels[index];
    return UMI_STATUS_OK;
}

/* Copy one complete row for encoders without lending the surface allocation. */
UmiStatus umi_media_image_surface_read_row(
    const UmiMediaImageSurface *surface,
    size_t y,
    UmiMediaRgbaPixel *out_pixels,
    size_t pixel_capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || out_pixels == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (y >= surface->height) return UMI_STATUS_NOT_FOUND;
    /* The caller must provide a complete row to avoid hidden partial output. */
    if (pixel_capacity < surface->width) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out_pixels, &surface->pixels[y * surface->width],
                 surface->width * sizeof(*out_pixels));
    return UMI_STATUS_OK;
}

/* Copy dimensions and revision without exposing pixel ownership. */
UmiStatus umi_media_image_surface_snapshot(
    const UmiMediaImageSurface *surface,
    UmiMediaImageSurfaceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (surface == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    out_snapshot->api_version = 1U;
    out_snapshot->width = surface->width;
    out_snapshot->height = surface->height;
    out_snapshot->pixel_count = surface->pixel_count;
    out_snapshot->revision = surface->revision;
    return UMI_STATUS_OK;
}

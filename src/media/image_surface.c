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
#include "umicom/media/image_edit.h"
#include "umicom/media/image_composite.h"

#include <stdlib.h>
#include <stdbool.h>
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
    /* Match the decoder row boundary: exhausted revisions refuse the entire
     * mutation before any pixel changes, rather than wrapping observation identity. */
    if (surface->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
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
    /* Keep revision identity monotonic just as complete row writes do. */
    if (surface->revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
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


/* Import complete decoder rows through the existing pixel owner. Explicit
 * channels preserve portable RGBA ordering without lending writable storage. */
UmiStatus UmiMediaImageSurfaceWriteRgbaRow(UmiMediaImageSurface *surface, size_t y, const void *rgba,
                                           size_t byte_count)
{
    if (surface == NULL || rgba == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (y >= surface->height)
        return UMI_STATUS_NOT_FOUND;
    if (surface->width > SIZE_MAX / 4U || byte_count != surface->width * 4U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (surface->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const unsigned char *bytes = rgba;
    for (size_t x = 0U; x < surface->width; ++x)
    {
        UmiMediaRgbaPixel *pixel = &surface->pixels[y * surface->width + x];
        pixel->red = bytes[x * 4U];
        pixel->green = bytes[x * 4U + 1U];
        pixel->blue = bytes[x * 4U + 2U];
        pixel->alpha = bytes[x * 4U + 3U];
    }
    surface->revision += 1U;
    return UMI_STATUS_OK;
}


/* Keep transformations in the surface owner so applications do not reinterpret
 * channel storage or mutate a source used by another view. Coordinates map
 * from the destination back into the crop, avoiding in-place pixel swaps. */
UmiStatus UmiMediaImageSurfaceApplyEdit(const UmiMediaImageSurface *source, const UmiMediaImageEdit *edit,
                                        const UmiCancellationToken *cancel,
                                        UmiMediaImageSurface **out_surface)
{
    if (source == NULL || edit == NULL || out_surface == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_surface != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (edit->width == 0U || edit->height == 0U || edit->x >= source->width || edit->y >= source->height ||
        edit->width > source->width - edit->x || edit->height > source->height - edit->y ||
        (unsigned)edit->orientation > (unsigned)UMI_MEDIA_IMAGE_FLIP_VERTICAL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (edit->height > UMI_MEDIA_IMAGE_EDIT_MAX_PIXELS / edit->width)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    bool quarter = edit->orientation == UMI_MEDIA_IMAGE_CLOCKWISE ||
                   edit->orientation == UMI_MEDIA_IMAGE_COUNTERCLOCKWISE;
    size_t width = quarter ? edit->height : edit->width, height = quarter ? edit->width : edit->height;
    UmiMediaImageSurface *result = NULL;
    UmiStatus status = umi_media_image_surface_create(width, height, &result);
    if (status != UMI_STATUS_OK)
        return status;
    for (size_t y = 0U; y < height; ++y)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            umi_media_image_surface_destroy(result);
            return UMI_STATUS_CANCELLED;
        }
        for (size_t x = 0U; x < width; ++x)
        {
            size_t source_x = x, source_y = y;
            switch (edit->orientation)
            {
            case UMI_MEDIA_IMAGE_CLOCKWISE:
                source_x = y;
                source_y = edit->height - 1U - x;
                break;
            case UMI_MEDIA_IMAGE_HALF_TURN:
                source_x = edit->width - 1U - x;
                source_y = edit->height - 1U - y;
                break;
            case UMI_MEDIA_IMAGE_COUNTERCLOCKWISE:
                source_x = edit->width - 1U - y;
                source_y = x;
                break;
            case UMI_MEDIA_IMAGE_FLIP_HORIZONTAL:
                source_x = edit->width - 1U - x;
                break;
            case UMI_MEDIA_IMAGE_FLIP_VERTICAL:
                source_y = edit->height - 1U - y;
                break;
            default:
                break;
            }
            result->pixels[y * width + x] =
                source->pixels[(edit->y + source_y) * source->width + edit->x + source_x];
        }
    }
    if (umi_cancellation_token_is_requested(cancel))
    {
        umi_media_image_surface_destroy(result);
        return UMI_STATUS_CANCELLED;
    }
    *out_surface = result;
    return UMI_STATUS_OK;
}

/* Work in exact straight-alpha sample values. The unrounded alpha denominator
 * avoids a second rounding error in RGB. Every product is bounded by 255^3,
 * safely below UINT32_MAX; transparent coverage preserves hidden source bytes. */
static UmiMediaRgbaPixel CompositePixel(UmiMediaRgbaPixel base, UmiMediaRgbaPixel over)
{
    if (over.alpha == 0U) return base;
    if (over.alpha == 255U) return over;
    uint32_t foreground = (uint32_t)over.alpha * 255U;
    uint32_t background = (uint32_t)base.alpha * (255U - (uint32_t)over.alpha);
    uint32_t alpha = foreground + background;
    UmiMediaRgbaPixel result;
    result.red = (uint8_t)(((uint32_t)over.red * foreground +
        (uint32_t)base.red * background + alpha / 2U) / alpha);
    result.green = (uint8_t)(((uint32_t)over.green * foreground +
        (uint32_t)base.green * background + alpha / 2U) / alpha);
    result.blue = (uint8_t)(((uint32_t)over.blue * foreground +
        (uint32_t)base.blue * background + alpha / 2U) / alpha);
    result.alpha = (uint8_t)((alpha + 127U) / 255U);
    return result;
}

UmiStatus UmiMediaImageSurfaceComposite(const UmiMediaImageSurface *source,
    const UmiMediaImageSurface *overlay, size_t x, size_t y,
    const UmiCancellationToken *cancel, UmiMediaImageSurface **out_surface)
{
    if (source == NULL || overlay == NULL || out_surface == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_surface != NULL) return UMI_STATUS_INVALID_STATE;
    /* Subtract after admitting the position; x+width must never overflow.
     * Opaque surfaces already own positive, multiplication-checked dimensions. */
    if (x >= source->width || y >= source->height ||
        overlay->width > source->width - x || overlay->height > source->height - y)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (source->pixel_count > UMI_MEDIA_IMAGE_EDIT_MAX_PIXELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiMediaImageEdit copy = {0U, 0U, source->width, source->height, UMI_MEDIA_IMAGE_ORIGINAL};
    UmiMediaImageSurface *candidate = NULL;
    /* Reuse the existing bounded, cancellable copy owner. Native and headless
     * clients share this publication rule rather than editing borrowed pixels. */
    UmiStatus status = UmiMediaImageSurfaceApplyEdit(source, &copy, cancel, &candidate);
    if (status != UMI_STATUS_OK) return status;
    for (size_t row = 0U; row < overlay->height; ++row) {
        if (umi_cancellation_token_is_requested(cancel)) {
            umi_media_image_surface_destroy(candidate);
            return UMI_STATUS_CANCELLED;
        }
        for (size_t column = 0U; column < overlay->width; ++column) {
            size_t destination = (y + row) * candidate->width + x + column;
            candidate->pixels[destination] = CompositePixel(candidate->pixels[destination],
                overlay->pixels[row * overlay->width + column]);
        }
    }
    if (umi_cancellation_token_is_requested(cancel)) {
        umi_media_image_surface_destroy(candidate);
        return UMI_STATUS_CANCELLED;
    }
    *out_surface = candidate;
    return UMI_STATUS_OK;
}

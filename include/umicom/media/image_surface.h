/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/media/image_surface.h
 *
 * PURPOSE:
 *   Define an owned RGBA image surface for image generation, thumbnails,
 *   animation frames, game assets and toolkit-neutral rendering adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_MEDIA_IMAGE_SURFACE_H
#define UMICOM_MEDIA_IMAGE_SURFACE_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Pixels use explicit channels so byte order is never inferred from an integer. */
typedef struct UmiMediaRgbaPixel {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} UmiMediaRgbaPixel;

/* The opaque surface owns a checked width-by-height pixel allocation. */
typedef struct UmiMediaImageSurface UmiMediaImageSurface;

/* Image snapshots describe storage without exposing a writable pixel pointer. */
typedef struct UmiMediaImageSurfaceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    size_t width;
    size_t height;
    size_t pixel_count;
    uint64_t revision;
} UmiMediaImageSurfaceSnapshot;

/* Create a zero-initialised surface after checking all size multiplications. */
UmiStatus umi_media_image_surface_create(
    size_t width,
    size_t height,
    UmiMediaImageSurface **out_surface);

/* Destroy the surface and its owned pixel allocation. */
void umi_media_image_surface_destroy(UmiMediaImageSurface *surface);

/* Fill every pixel with one colour and publish a new revision. */
UmiStatus umi_media_image_surface_clear(
    UmiMediaImageSurface *surface,
    UmiMediaRgbaPixel pixel);

/* Replace one checked pixel coordinate. */
UmiStatus umi_media_image_surface_set_pixel(
    UmiMediaImageSurface *surface,
    size_t x,
    size_t y,
    UmiMediaRgbaPixel pixel);

/* Copy one checked pixel coordinate into caller-owned storage. */
UmiStatus umi_media_image_surface_get_pixel(
    const UmiMediaImageSurface *surface,
    size_t x,
    size_t y,
    UmiMediaRgbaPixel *out_pixel);

/* Copy a row into caller memory for encoders and GUI upload adapters. */
UmiStatus umi_media_image_surface_read_row(
    const UmiMediaImageSurface *surface,
    size_t y,
    UmiMediaRgbaPixel *out_pixels,
    size_t pixel_capacity);

/* Copy dimensions and revision without lending the owned pixel buffer. */
UmiStatus umi_media_image_surface_snapshot(
    const UmiMediaImageSurface *surface,
    UmiMediaImageSurfaceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif

#endif

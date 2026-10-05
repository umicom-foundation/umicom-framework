/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media/png_encode.c
 * PURPOSE: Encode complete owned Media surfaces with bounded PNG output and shared native codec memory accounting.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media/png_image.h"
#include "png_memory.h"
#include <stdlib.h>
#include <string.h>
#if UMI_PNG_MEMORY_AVAILABLE && defined(PNG_WRITE_SUPPORTED)
#define UMI_PNG_ENCODER_AVAILABLE 1
#else
#define UMI_PNG_ENCODER_AVAILABLE 0
#endif
int UmiMediaPngEncoderAvailable(void) { return UMI_PNG_ENCODER_AVAILABLE; }
void UmiMediaPngFree(void *bytes) { free(bytes); }
#if UMI_PNG_ENCODER_AVAILABLE
typedef struct PngWrite
{
    UmiPngMemory memory;
    png_structp png;
    png_infop info;
    const UmiCancellationToken *cancel;
    unsigned char *bytes, *row;
    UmiMediaRgbaPixel *pixels;
    size_t size, capacity;
} PngWrite;
static void Append(png_structp png, png_bytep bytes, png_size_t size)
{
    PngWrite *writer = png_get_io_ptr(png);
    if (umi_cancellation_token_is_requested(writer->cancel))
    {
        writer->memory.status = UMI_STATUS_CANCELLED;
        png_error(png, "cancelled");
    }
    if (size > UMI_MEDIA_PNG_INPUT_LIMIT - writer->size)
    {
        writer->memory.status = UMI_STATUS_CAPACITY_EXCEEDED;
        png_error(png, "encoded image limit");
    }
    size_t required = writer->size + size;
    if (required > writer->capacity)
    {
        size_t capacity = writer->capacity == 0U ? 65536U : writer->capacity;
        while (capacity < required)
            capacity = capacity > UMI_MEDIA_PNG_INPUT_LIMIT / 2U ? UMI_MEDIA_PNG_INPUT_LIMIT : capacity * 2U;
        unsigned char *grown = realloc(writer->bytes, capacity);
        if (grown == NULL)
        {
            writer->memory.status = UMI_STATUS_OUT_OF_MEMORY;
            png_error(png, "output allocation");
        }
        writer->bytes = grown;
        writer->capacity = capacity;
    }
    if (size != 0U)
        memcpy(writer->bytes + writer->size, bytes, size);
    writer->size += size;
}
static void Flush(png_structp png) { (void)png; }
static void Destroy(PngWrite *writer)
{
    png_destroy_write_struct(&writer->png, &writer->info);
    free(writer->bytes);
    free(writer->row);
    free(writer->pixels);
    free(writer);
}
static UmiStatus Encode(const UmiMediaImageSurface *surface, const UmiMediaImageSurfaceSnapshot *info,
                        const UmiCancellationToken *cancel, unsigned char **out_bytes, size_t *out_size)
{
    PngWrite *writer = calloc(1U, sizeof(*writer));
    if (writer == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    writer->cancel = cancel;
    writer->png = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, &writer->memory, UmiPngFailed, NULL,
                                            &writer->memory, UmiPngAllocate, UmiPngRelease);
    UmiStatus status;
    if (writer->png == NULL)
    {
        status = writer->memory.status == UMI_STATUS_OK ? UMI_STATUS_OUT_OF_MEMORY : writer->memory.status;
        Destroy(writer);
        return status;
    }
    /* Retain every mutable cleanup field in heap storage across libpng's
     * error jump. Output transfers only after png_write_end succeeds. */
    if (setjmp(png_jmpbuf(writer->png)) != 0)
    {
        status = writer->memory.status;
        Destroy(writer);
        return status;
    }
    png_set_error_fn(writer->png, &writer->memory, UmiPngFailed, UmiPngWarned);
    writer->info = png_create_info_struct(writer->png);
    if (writer->info == NULL)
    {
        status = writer->memory.status == UMI_STATUS_OK ? UMI_STATUS_OUT_OF_MEMORY : writer->memory.status;
        Destroy(writer);
        return status;
    }
    png_set_write_fn(writer->png, writer, Append, Flush);
    png_set_IHDR(writer->png, writer->info, (png_uint_32)info->width, (png_uint_32)info->height, 8,
                 PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(writer->png, writer->info);
    writer->row = malloc(info->width * 4U);
    writer->pixels = malloc(info->width * sizeof(*writer->pixels));
    if (writer->row == NULL || writer->pixels == NULL)
    {
        Destroy(writer);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t y = 0U; y < info->height; ++y)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            Destroy(writer);
            return UMI_STATUS_CANCELLED;
        }
        status = umi_media_image_surface_read_row(surface, y, writer->pixels, info->width);
        if (status != UMI_STATUS_OK)
        {
            Destroy(writer);
            return status;
        }
        for (size_t x = 0U; x < info->width; ++x)
        {
            writer->row[x * 4U] = writer->pixels[x].red;
            writer->row[x * 4U + 1U] = writer->pixels[x].green;
            writer->row[x * 4U + 2U] = writer->pixels[x].blue;
            writer->row[x * 4U + 3U] = writer->pixels[x].alpha;
        }
        png_write_row(writer->png, writer->row);
    }
    png_write_end(writer->png, writer->info);
    if (umi_cancellation_token_is_requested(cancel))
    {
        Destroy(writer);
        return UMI_STATUS_CANCELLED;
    }
    *out_bytes = writer->bytes;
    *out_size = writer->size;
    writer->bytes = NULL;
    Destroy(writer);
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiMediaPngEncode(const UmiMediaImageSurface *surface, const UmiCancellationToken *cancel,
                            unsigned char **out_bytes, size_t *out_size)
{
    if (surface == NULL || out_bytes == NULL || out_size == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_bytes != NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiMediaImageSurfaceSnapshot info;
    UmiStatus status = umi_media_image_surface_snapshot(surface, &info);
    if (status != UMI_STATUS_OK)
        return status;
    if (info.width > UMI_MEDIA_PNG_MAX_DIMENSION || info.height > UMI_MEDIA_PNG_MAX_DIMENSION ||
        info.pixel_count > UMI_MEDIA_PNG_MAX_PIXELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
#if UMI_PNG_ENCODER_AVAILABLE
    return Encode(surface, &info, cancel, out_bytes, out_size);
#else
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}

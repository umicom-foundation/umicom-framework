/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media/png_image.c
 * PURPOSE: Use optional libpng for bounded static image decoding while keeping pixel ownership in the existing Media engine.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media/png_image.h"
#include "png_memory.h"
#include <stdlib.h>
#include <string.h>
#ifdef UMICOM_MEDIA_PNG_ENABLED
#include <png.h>
#include <zlib.h>
#endif
/* A reduced libpng build must not silently bypass a memory or transform rule. */
#if defined(UMICOM_MEDIA_PNG_ENABLED) && defined(PNG_READ_SUPPORTED) && defined(PNG_USER_MEM_SUPPORTED) &&   \
    defined(PNG_SETJMP_SUPPORTED) && defined(PNG_SET_USER_LIMITS_SUPPORTED) &&                               \
    defined(PNG_READ_TRANSFORMS_SUPPORTED) && defined(PNG_READ_EXPAND_SUPPORTED) &&                          \
    defined(PNG_READ_GRAY_TO_RGB_SUPPORTED) && defined(PNG_READ_FILLER_SUPPORTED) &&                         \
    defined(PNG_READ_STRIP_16_TO_8_SUPPORTED)
#define UMI_PNG_DECODER_AVAILABLE 1
#else
#define UMI_PNG_DECODER_AVAILABLE 0
#endif
int UmiMediaPngAvailable(void) { return UMI_PNG_DECODER_AVAILABLE; }
#if UMI_PNG_DECODER_AVAILABLE
typedef struct PngRead
{
    const unsigned char *bytes;
    size_t size, offset;
    const UmiCancellationToken *cancel;
    UmiPngMemory memory;
    png_structp png;
    png_infop info;
    UmiMediaImageSurface *surface;
    unsigned char *row;
} PngRead;
static void ReadBytes(png_structp png, png_bytep out, png_size_t size)
{
    PngRead *reader = png_get_io_ptr(png);
    if (umi_cancellation_token_is_requested(reader->cancel))
    {
        reader->memory.status = UMI_STATUS_CANCELLED;
        png_error(png, "cancelled");
    }
    if (size > reader->size - reader->offset)
        png_error(png, "incomplete input");
    memcpy(out, reader->bytes + reader->offset, size);
    reader->offset += size;
}
static uint32_t BigEndian(const unsigned char *bytes)
{
    return ((uint32_t)bytes[0] << 24U) | ((uint32_t)bytes[1] << 16U) | ((uint32_t)bytes[2] << 8U) | bytes[3];
}
static UmiStatus Envelope(const unsigned char *bytes, size_t size, const UmiCancellationToken *cancel)
{
    if (size < 33U || png_sig_cmp(bytes, 0U, 8U) != 0)
        return UMI_STATUS_PARSE_ERROR;
    size_t offset = 8U, chunks = 0U;
    while (offset < size)
    {
        if (umi_cancellation_token_is_requested(cancel))
            return UMI_STATUS_CANCELLED;
        if (++chunks > 4096U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if (size - offset < 12U)
            return UMI_STATUS_PARSE_ERROR;
        size_t count = (size_t)BigEndian(bytes + offset);
        if (count > size - offset - 12U)
            return UMI_STATUS_PARSE_ERROR;
        const unsigned char *kind = bytes + offset + 4U;
        if (chunks == 1U && (count != 13U || memcmp(kind, "IHDR", 4U) != 0))
            return UMI_STATUS_PARSE_ERROR;
        /* An animated PNG must not silently turn into its first frame. */
        if (memcmp(kind, "acTL", 4U) == 0 || memcmp(kind, "fcTL", 4U) == 0 || memcmp(kind, "fdAT", 4U) == 0)
            return UMI_STATUS_NOT_IMPLEMENTED;
        uLong crc = crc32(0L, Z_NULL, 0);
        for (size_t done = 0U; done < count + 4U;)
        {
            if (umi_cancellation_token_is_requested(cancel))
                return UMI_STATUS_CANCELLED;
            size_t part = count + 4U - done;
            if (part > 65536U)
                part = 65536U;
            crc = crc32(crc, kind + done, (uInt)part);
            done += part;
        }
        if ((uint32_t)crc != BigEndian(bytes + offset + 8U + count))
            return UMI_STATUS_PARSE_ERROR;
        offset += count + 12U;
        if (memcmp(kind, "IEND", 4U) == 0)
            return count == 0U && offset == size ? UMI_STATUS_OK : UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_PARSE_ERROR;
}
static void Destroy(PngRead *reader)
{
    png_destroy_read_struct(&reader->png, &reader->info, NULL);
    free(reader->row);
    umi_media_image_surface_destroy(reader->surface);
    free(reader);
}
static UmiStatus Decode(const unsigned char *bytes, size_t byte_count, const UmiCancellationToken *cancel,
                        UmiMediaImageSurface **out_surface)
{
    UmiStatus status = Envelope(bytes, byte_count, cancel);
    if (status != UMI_STATUS_OK)
        return status;
    uint32_t width = BigEndian(bytes + 16U), height = BigEndian(bytes + 20U);
    if (width == 0U || height == 0U)
        return UMI_STATUS_PARSE_ERROR;
    if (width > UMI_MEDIA_PNG_MAX_DIMENSION || height > UMI_MEDIA_PNG_MAX_DIMENSION ||
        (uint64_t)width * height > UMI_MEDIA_PNG_MAX_PIXELS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (bytes[28U] != 0U)
        return UMI_STATUS_NOT_IMPLEMENTED; /* Interlaced input needs a separate whole-image owner. */
    PngRead *reader = calloc(1U, sizeof(*reader));
    if (reader == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    reader->bytes = bytes;
    reader->size = byte_count;
    reader->cancel = cancel;
    reader->png = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, &reader->memory, UmiPngFailed, NULL,
                                           &reader->memory, UmiPngAllocate, UmiPngRelease);
    if (reader->png == NULL)
    {
        status = reader->memory.status == UMI_STATUS_OK ? UMI_STATUS_OUT_OF_MEMORY : reader->memory.status;
        Destroy(reader);
        return status;
    }
    /* All state used after libpng's longjmp lives on the heap. C local variables
     * changed after setjmp are otherwise indeterminate on an error return. */
    if (setjmp(png_jmpbuf(reader->png)) != 0)
    {
        status = reader->memory.status;
        Destroy(reader);
        return status;
    }
    png_set_error_fn(reader->png, &reader->memory, UmiPngFailed, UmiPngWarned);
    reader->info = png_create_info_struct(reader->png);
    if (reader->info == NULL)
    {
        status = reader->memory.status == UMI_STATUS_OK ? UMI_STATUS_OUT_OF_MEMORY : reader->memory.status;
        Destroy(reader);
        return status;
    }
    png_set_read_fn(reader->png, reader, ReadBytes);
    png_set_user_limits(reader->png, UMI_MEDIA_PNG_MAX_DIMENSION, UMI_MEDIA_PNG_MAX_DIMENSION);
    png_set_chunk_cache_max(reader->png, 64U);
    png_set_chunk_malloc_max(reader->png, 1024U * 1024U);
    png_set_crc_action(reader->png, PNG_CRC_ERROR_QUIT, PNG_CRC_ERROR_QUIT);
    png_read_info(reader->png, reader->info);
    int depth = png_get_bit_depth(reader->png, reader->info),
        colour = png_get_color_type(reader->png, reader->info);
    int transparency = png_get_valid(reader->png, reader->info, PNG_INFO_tRNS) != 0U;
    if (depth == 16)
        png_set_strip_16(reader->png);
    if (colour == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(reader->png);
    if (colour == PNG_COLOR_TYPE_GRAY && depth < 8)
        png_set_expand_gray_1_2_4_to_8(reader->png);
    if (transparency)
        png_set_tRNS_to_alpha(reader->png);
    if (colour == PNG_COLOR_TYPE_GRAY || colour == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(reader->png);
    if ((colour & PNG_COLOR_MASK_ALPHA) == 0 && !transparency)
        png_set_add_alpha(reader->png, 255U, PNG_FILLER_AFTER);
    png_read_update_info(reader->png, reader->info);
    size_t row_size = (size_t)width * 4U;
    if (png_get_rowbytes(reader->png, reader->info) != row_size ||
        png_get_channels(reader->png, reader->info) != 4 || png_get_bit_depth(reader->png, reader->info) != 8)
        png_error(reader->png, "unsupported row layout");
    reader->memory.status = umi_media_image_surface_create((size_t)width, (size_t)height, &reader->surface);
    if (reader->memory.status != UMI_STATUS_OK)
    {
        status = reader->memory.status;
        Destroy(reader);
        return status;
    }
    reader->row = malloc(row_size);
    if (reader->row == NULL)
    {
        Destroy(reader);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    for (size_t y = 0U; y < (size_t)height; ++y)
    {
        if (umi_cancellation_token_is_requested(cancel))
        {
            Destroy(reader);
            return UMI_STATUS_CANCELLED;
        }
        png_read_row(reader->png, reader->row, NULL);
        reader->memory.status = UmiMediaImageSurfaceWriteRgbaRow(reader->surface, y, reader->row, row_size);
        if (reader->memory.status != UMI_STATUS_OK)
        {
            status = reader->memory.status;
            Destroy(reader);
            return status;
        }
    }
    png_read_end(reader->png, reader->info);
    if (reader->offset != reader->size)
    {
        Destroy(reader);
        return UMI_STATUS_PARSE_ERROR;
    }
    if (umi_cancellation_token_is_requested(cancel))
    {
        Destroy(reader);
        return UMI_STATUS_CANCELLED;
    }
    *out_surface = reader->surface;
    reader->surface = NULL;
    Destroy(reader);
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiMediaPngDecode(const void *bytes, size_t byte_count, const UmiCancellationToken *cancel,
                            UmiMediaImageSurface **out_surface)
{
    if (bytes == NULL || out_surface == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_surface != NULL)
        return UMI_STATUS_INVALID_STATE;
    if (byte_count > UMI_MEDIA_PNG_INPUT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
#if UMI_PNG_DECODER_AVAILABLE
    return Decode(bytes, byte_count, cancel, out_surface);
#else
    return UMI_STATUS_NOT_IMPLEMENTED;
#endif
}

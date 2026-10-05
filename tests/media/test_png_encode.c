/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_png_encode.c
 * PURPOSE: Check exact PNG pixel roundtrips, output ownership and encoder refusal before publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media/png_image.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #x);                                                  \
            return 1;                                                                                        \
        }                                                                                                    \
    } while (0)
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *cases[] = {"roundtrip",       "deterministic", "independent", "null-surface",
                                            "null-pointer",    "null-size",     "live-output", "cancelled",
                                            "dimension-limit", "pixel-limit",   "unavailable"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    UmiMediaImageSurface *image = NULL, *decoded = NULL;
    size_t width = strcmp(mode, "dimension-limit") == 0 ? 8193U
                   : strcmp(mode, "pixel-limit") == 0   ? 4097U
                                                        : 2U;
    size_t height = strcmp(mode, "pixel-limit") == 0 ? 4096U : 1U;
    CHECK(umi_media_image_surface_create(width, height, &image) == UMI_STATUS_OK);
    const unsigned char pixels[] = {20U, 40U, 80U, 128U, 255U, 10U, 0U, 0U};
    if (width == 2U)
        CHECK(UmiMediaImageSurfaceWriteRgbaRow(image, 0U, pixels, sizeof(pixels)) == UMI_STATUS_OK);
    UmiMediaImageSurfaceSnapshot before, after;
    CHECK(umi_media_image_surface_snapshot(image, &before) == UMI_STATUS_OK);
    unsigned char *bytes = NULL;
    size_t size = 789U;
    UmiStatus expected = UMI_STATUS_OK;
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "unavailable") == 0)
    {
        if (UmiMediaPngEncoderAvailable())
        {
            umi_media_image_surface_destroy(image);
            umi_cancellation_token_destroy(cancel);
            return 77;
        }
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    }
    else if (!UmiMediaPngEncoderAvailable() || !UmiMediaPngAvailable())
    {
        umi_media_image_surface_destroy(image);
        umi_cancellation_token_destroy(cancel);
        return 77;
    }
    if (strcmp(mode, "null-surface") == 0 || strcmp(mode, "null-pointer") == 0 ||
        strcmp(mode, "null-size") == 0)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "dimension-limit") == 0 || strcmp(mode, "pixel-limit") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "live-output") == 0)
    {
        unsigned char sentinel = 5U;
        bytes = &sentinel;
        CHECK(UmiMediaPngEncode(image, NULL, &bytes, &size) == UMI_STATUS_INVALID_STATE &&
              bytes == &sentinel && size == 789U && sentinel == 5U);
        bytes = NULL;
    }
    else
    {
        CHECK(UmiMediaPngEncode(strcmp(mode, "null-surface") == 0 ? NULL : image, cancel,
                                strcmp(mode, "null-pointer") == 0 ? NULL : &bytes,
                                strcmp(mode, "null-size") == 0 ? NULL : &size) == expected);
        if (expected == UMI_STATUS_OK)
        {
            CHECK(size > 8U && memcmp(bytes, "\x89PNG\r\n\x1a\n", 8U) == 0);
            if (strcmp(mode, "deterministic") == 0)
            {
                unsigned char *other = NULL;
                size_t other_size = 0U;
                CHECK(UmiMediaPngEncode(image, NULL, &other, &other_size) == UMI_STATUS_OK &&
                      other_size == size && memcmp(bytes, other, size) == 0);
                UmiMediaPngFree(other);
            }
            CHECK(UmiMediaPngDecode(bytes, size, NULL, &decoded) == UMI_STATUS_OK);
            UmiMediaPngFree(bytes);
            bytes = NULL;
            if (strcmp(mode, "independent") == 0)
                CHECK(umi_media_image_surface_clear(image, (UmiMediaRgbaPixel){0U, 0U, 0U, 0U}) ==
                      UMI_STATUS_OK);
            for (size_t x = 0U; x < 2U; ++x)
            {
                UmiMediaRgbaPixel pixel;
                CHECK(umi_media_image_surface_get_pixel(decoded, x, 0U, &pixel) == UMI_STATUS_OK);
                CHECK(pixel.red == pixels[x * 4U] && pixel.green == pixels[x * 4U + 1U] &&
                      pixel.blue == pixels[x * 4U + 2U] && pixel.alpha == pixels[x * 4U + 3U]);
            }
        }
        else
            CHECK(bytes == NULL && size == 789U);
    }
    CHECK(umi_media_image_surface_snapshot(image, &after) == UMI_STATUS_OK &&
          after.revision == before.revision + (strcmp(mode, "independent") == 0 ? 1U : 0U));
    UmiMediaPngFree(bytes);
    UmiMediaPngFree(NULL);
    umi_media_image_surface_destroy(image);
    umi_media_image_surface_destroy(decoded);
    umi_cancellation_token_destroy(cancel);
    return 0;
}

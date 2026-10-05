/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_png_image.c
 * PURPOSE: Check pixel values and malformed complete inputs at the optional portable PNG decoder boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "png_fixture.h"
#include "umicom/media/png_image.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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
    const char *mode = argv[1], *cases[] = {"rgba",
                                            "rgb",
                                            "gray",
                                            "gray-alpha",
                                            "palette",
                                            "gray-bit",
                                            "gray-wide",
                                            "transparent",
                                            "interlaced",
                                            "animated",
                                            "wide",
                                            "pixels",
                                            "zero",
                                            "depth",
                                            "broken-deflate",
                                            "signature",
                                            "crc",
                                            "truncated",
                                            "trailing",
                                            "missing-end",
                                            "unknown-critical",
                                            "many-chunks",
                                            "owned",
                                            "cancelled",
                                            "live-output",
                                            "null-output",
                                            "null-input",
                                            "input-limit",
                                            "unavailable",
                                            "row-import",
                                            "row-order",
                                            "row-short",
                                            "row-long",
                                            "row-index",
                                            "row-null-input",
                                            "row-null-surface"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    if (strncmp(mode, "row-", 4U) == 0)
    {
        UmiMediaImageSurface *surface = NULL;
        CHECK(umi_media_image_surface_create(2U, 2U, &surface) == UMI_STATUS_OK);
        const unsigned char first[] = {1U, 2U, 3U, 4U, 10U, 20U, 30U, 40U};
        const unsigned char second[] = {5U, 6U, 7U, 8U, 50U, 60U, 70U, 80U, 0U};
        CHECK(UmiMediaImageSurfaceWriteRgbaRow(surface, 0U, first, sizeof(first)) == UMI_STATUS_OK);
        UmiMediaImageSurfaceSnapshot before, after;
        CHECK(umi_media_image_surface_snapshot(surface, &before) == UMI_STATUS_OK);
        size_t y = strcmp(mode, "row-index") == 0 ? 2U : strcmp(mode, "row-order") == 0 ? 1U : 0U;
        size_t size = strcmp(mode, "row-short") == 0 ? 7U : strcmp(mode, "row-long") == 0 ? 9U : 8U;
        UmiStatus status =
            UmiMediaImageSurfaceWriteRgbaRow(strcmp(mode, "row-null-surface") == 0 ? NULL : surface, y,
                                             strcmp(mode, "row-null-input") == 0 ? NULL : second, size);
        bool success = strcmp(mode, "row-import") == 0 || strcmp(mode, "row-order") == 0;
        CHECK(status == (success                          ? UMI_STATUS_OK
                         : strcmp(mode, "row-index") == 0 ? UMI_STATUS_NOT_FOUND
                                                          : UMI_STATUS_INVALID_ARGUMENT));
        CHECK(umi_media_image_surface_snapshot(surface, &after) == UMI_STATUS_OK &&
              after.revision == before.revision + (success ? 1U : 0U));
        UmiMediaRgbaPixel pixel;
        CHECK(umi_media_image_surface_get_pixel(surface, 0U, 0U, &pixel) == UMI_STATUS_OK);
        CHECK(pixel.red == (strcmp(mode, "row-import") == 0 ? 5U : 1U) &&
              pixel.alpha == (strcmp(mode, "row-import") == 0 ? 8U : 4U));
        if (strcmp(mode, "row-order") == 0)
        {
            CHECK(umi_media_image_surface_get_pixel(surface, 1U, 1U, &pixel) == UMI_STATUS_OK &&
                  pixel.red == 50U && pixel.green == 60U && pixel.blue == 70U && pixel.alpha == 80U);
        }
        umi_media_image_surface_destroy(surface);
        return 0;
    }
    TestPngFixture input = PngFixture(mode);
    if (input.bytes == NULL)
        input = PngFixture("rgba");
    UmiMediaImageSurface *image = NULL;
    if (strcmp(mode, "unavailable") == 0)
    {
        if (UmiMediaPngAvailable())
            return 77;
        CHECK(UmiMediaPngDecode(input.bytes, input.size, NULL, &image) == UMI_STATUS_NOT_IMPLEMENTED &&
              image == NULL);
        return 0;
    }
    if (!UmiMediaPngAvailable())
        return 77;
    if (strcmp(mode, "null-input") == 0)
    {
        CHECK(UmiMediaPngDecode(NULL, input.size, NULL, &image) == UMI_STATUS_INVALID_ARGUMENT &&
              image == NULL);
        return 0;
    }
    if (strcmp(mode, "null-output") == 0)
    {
        CHECK(UmiMediaPngDecode(input.bytes, input.size, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        return 0;
    }
    if (strcmp(mode, "input-limit") == 0)
    {
        CHECK(UmiMediaPngDecode(input.bytes, UMI_MEDIA_PNG_INPUT_LIMIT + 1U, NULL, &image) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              image == NULL);
        return 0;
    }
    if (strcmp(mode, "live-output") == 0)
    {
        CHECK(umi_media_image_surface_create(1U, 1U, &image) == UMI_STATUS_OK);
        UmiMediaImageSurface *same = image;
        CHECK(UmiMediaPngDecode(input.bytes, input.size, NULL, &image) == UMI_STATUS_INVALID_STATE &&
              image == same);
        umi_media_image_surface_destroy(image);
        return 0;
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    unsigned char *bytes = malloc(input.size + 1U);
    CHECK(bytes != NULL);
    memcpy(bytes, input.bytes, input.size);
    size_t count = input.size;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "signature") == 0)
    {
        bytes[0] = 0U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "crc") == 0)
    {
        bytes[29] ^= 1U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "trailing") == 0)
    {
        bytes[count++] = 0U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "missing-end") == 0)
    {
        count -= 12U;
        expected = UMI_STATUS_PARSE_ERROR;
    }
    if (strcmp(mode, "truncated") == 0)
    {
        for (size_t i = 0U; i < input.size; ++i)
            CHECK(UmiMediaPngDecode(bytes, i, NULL, &image) == UMI_STATUS_PARSE_ERROR && image == NULL);
    }
    if (strcmp(mode, "interlaced") == 0 || strcmp(mode, "animated") == 0)
        expected = UMI_STATUS_NOT_IMPLEMENTED;
    if (strcmp(mode, "wide") == 0 || strcmp(mode, "pixels") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(mode, "zero") == 0 || strcmp(mode, "depth") == 0 || strcmp(mode, "broken-deflate") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    if (strcmp(mode, "unknown-critical") == 0 || strcmp(mode, "many-chunks") == 0)
    {
        /* Explicit chunk fixtures include correct CRCs so rejection tests the
         * critical-chunk policy or count bound, not an earlier checksum error. */
        static const unsigned char unknown[] = {0x00U, 0x00U, 0x00U, 0x00U, 0x41U, 0x42U,
                                                0x43U, 0x44U, 0xdbU, 0x17U, 0x20U, 0xa5U};
        static const unsigned char ancillary[] = {0x00U, 0x00U, 0x00U, 0x00U, 0x72U, 0x75U,
                                                  0x53U, 0x74U, 0x74U, 0xb6U, 0xa6U, 0x02U};
        size_t inserted = strcmp(mode, "many-chunks") == 0 ? 4096U : 1U;
        const unsigned char *chunk = inserted == 1U ? unknown : ancillary;
        free(bytes);
        count = input.size + inserted * 12U;
        bytes = malloc(count);
        CHECK(bytes != NULL);
        memcpy(bytes, input.bytes, 33U);
        for (size_t i = 0U; i < inserted; ++i)
            memcpy(bytes + 33U + i * 12U, chunk, 12U);
        memcpy(bytes + 33U + inserted * 12U, input.bytes + 33U, input.size - 33U);
        expected = inserted == 1U ? UMI_STATUS_PARSE_ERROR : UMI_STATUS_CAPACITY_EXCEEDED;
    }
    CHECK(UmiMediaPngDecode(bytes, count, cancel, &image) == expected);
    if (expected == UMI_STATUS_OK)
    {
        /* The input can disappear immediately after success. */
        memset(bytes, 0, count);
        free(bytes);
        bytes = NULL;
        UmiMediaImageSurfaceSnapshot info;
        CHECK(umi_media_image_surface_snapshot(image, &info) == UMI_STATUS_OK && info.width == 2U);
        UmiMediaRgbaPixel left, right;
        CHECK(umi_media_image_surface_get_pixel(image, 0U, 0U, &left) == UMI_STATUS_OK);
        CHECK(umi_media_image_surface_get_pixel(image, 1U, 0U, &right) == UMI_STATUS_OK);
        if (strcmp(mode, "gray") == 0)
            CHECK(left.red == 40U && left.green == 40U && left.blue == 40U && left.alpha == 255U &&
                  right.red == 200U);
        else if (strcmp(mode, "gray-alpha") == 0)
            CHECK(left.red == 50U && left.green == 50U && left.blue == 50U && left.alpha == 100U &&
                  right.red == 200U && right.alpha == 255U);
        else if (strcmp(mode, "gray-bit") == 0)
            CHECK(left.red == 0U && right.red == 255U && left.alpha == 255U);
        else if (strcmp(mode, "gray-wide") == 0)
            CHECK(left.red == 18U && right.red == 171U && left.alpha == 255U);
        else
        {
            CHECK(left.red == 255U && left.green == 0U && left.blue == 0U && right.red == 0U &&
                  right.green == 255U && right.blue == 0U);
            CHECK(left.alpha == (strcmp(mode, "transparent") == 0 ? 0U : 255U));
            CHECK(right.alpha ==
                  (strcmp(mode, "rgb") == 0 || strcmp(mode, "transparent") == 0 ? 255U : 128U));
            if (info.height == 2U)
            {
                CHECK(umi_media_image_surface_get_pixel(image, 0U, 1U, &left) == UMI_STATUS_OK &&
                      left.blue == 255U && left.alpha == 0U);
                CHECK(umi_media_image_surface_get_pixel(image, 1U, 1U, &right) == UMI_STATUS_OK &&
                      right.red == 255U && right.green == 255U && right.blue == 255U && right.alpha == 255U);
            }
        }
    }
    else
        CHECK(image == NULL);
    free(bytes);
    umi_media_image_surface_destroy(image);
    umi_cancellation_token_destroy(cancel);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_image_edit.c
 * PURPOSE: Check exact coordinate results, original-source preservation and complete image edit refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media/image_edit.h"
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
    const char *mode = argv[1], *cases[] = {"identity",         "crop",
                                            "crop-clockwise",   "clockwise",
                                            "counterclockwise", "half-turn",
                                            "flip-horizontal",  "flip-vertical",
                                            "one-pixel",        "empty-width",
                                            "empty-height",     "outside-x",
                                            "outside-y",        "oversized-width",
                                            "oversized-height", "invalid-orientation",
                                            "cancelled",        "live-output",
                                            "null-source",      "null-edit",
                                            "null-output",      "pixel-limit"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    UmiMediaImageSurface *source = NULL, *result = NULL;
    CHECK(umi_media_image_surface_create(3U, 2U, &source) == UMI_STATUS_OK);
    for (size_t i = 0U; i < 6U; ++i)
    {
        UmiMediaRgbaPixel pixel = {(uint8_t)(i + 1U), (uint8_t)(i + 11U), (uint8_t)(i + 21U),
                                   (uint8_t)(i + 31U)};
        CHECK(umi_media_image_surface_set_pixel(source, i % 3U, i / 3U, pixel) == UMI_STATUS_OK);
    }
    UmiMediaImageSurfaceSnapshot before, after;
    CHECK(umi_media_image_surface_snapshot(source, &before) == UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiMediaImageEdit edit = {0U, 0U, 3U, 2U, UMI_MEDIA_IMAGE_ORIGINAL};
    const unsigned char original[] = {1, 2, 3, 4, 5, 6}, clockwise[] = {4, 1, 5, 2, 6, 3},
                        counterclockwise[] = {3, 6, 2, 5, 1, 4}, half[] = {6, 5, 4, 3, 2, 1},
                        horizontal[] = {3, 2, 1, 6, 5, 4}, vertical[] = {4, 5, 6, 1, 2, 3},
                        crop[] = {2, 3, 5, 6}, crop_clockwise[] = {5, 2, 6, 3}, one[] = {6};
    const unsigned char *expected = original;
    size_t width = 3U, height = 2U;
    UmiStatus expected_status = UMI_STATUS_OK;
    if (strcmp(mode, "clockwise") == 0)
    {
        edit.orientation = UMI_MEDIA_IMAGE_CLOCKWISE;
        expected = clockwise;
        width = 2U;
        height = 3U;
    }
    if (strcmp(mode, "counterclockwise") == 0)
    {
        edit.orientation = UMI_MEDIA_IMAGE_COUNTERCLOCKWISE;
        expected = counterclockwise;
        width = 2U;
        height = 3U;
    }
    if (strcmp(mode, "half-turn") == 0)
    {
        edit.orientation = UMI_MEDIA_IMAGE_HALF_TURN;
        expected = half;
    }
    if (strcmp(mode, "flip-horizontal") == 0)
    {
        edit.orientation = UMI_MEDIA_IMAGE_FLIP_HORIZONTAL;
        expected = horizontal;
    }
    if (strcmp(mode, "flip-vertical") == 0)
    {
        edit.orientation = UMI_MEDIA_IMAGE_FLIP_VERTICAL;
        expected = vertical;
    }
    if (strcmp(mode, "crop") == 0 || strcmp(mode, "crop-clockwise") == 0)
    {
        edit.x = 1U;
        edit.width = 2U;
        width = 2U;
        if (strcmp(mode, "crop-clockwise") == 0)
        {
            edit.orientation = UMI_MEDIA_IMAGE_CLOCKWISE;
            expected = crop_clockwise;
        }
        else
            expected = crop;
    }
    if (strcmp(mode, "one-pixel") == 0)
    {
        edit.x = 2U;
        edit.y = 1U;
        edit.width = 1U;
        edit.height = 1U;
        width = 1U;
        height = 1U;
        expected = one;
    }
    if (strcmp(mode, "empty-width") == 0)
    {
        edit.width = 0U;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "empty-height") == 0)
    {
        edit.height = 0U;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "outside-x") == 0)
    {
        edit.x = 3U;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "outside-y") == 0)
    {
        edit.y = 2U;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "oversized-width") == 0)
    {
        edit.width = SIZE_MAX;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "oversized-height") == 0)
    {
        edit.height = SIZE_MAX;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-orientation") == 0)
    {
        edit.orientation = (UmiMediaImageOrientation)-1;
        CHECK(UmiMediaImageSurfaceApplyEdit(source, &edit, NULL, &result) == UMI_STATUS_INVALID_ARGUMENT &&
              result == NULL);
        edit.orientation = (UmiMediaImageOrientation)99;
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "cancelled") == 0)
    {
        umi_cancellation_token_request(cancel);
        expected_status = UMI_STATUS_CANCELLED;
    }
    if (strcmp(mode, "null-source") == 0 || strcmp(mode, "null-edit") == 0 ||
        strcmp(mode, "null-output") == 0)
        expected_status = UMI_STATUS_INVALID_ARGUMENT;
    if (strcmp(mode, "live-output") == 0)
    {
        result = source;
        CHECK(UmiMediaImageSurfaceApplyEdit(source, &edit, NULL, &result) == UMI_STATUS_INVALID_STATE &&
              result == source);
        result = NULL;
    }
    else if (strcmp(mode, "pixel-limit") == 0)
    {
        UmiMediaImageSurface *large = NULL;
        CHECK(umi_media_image_surface_create(4097U, 4096U, &large) == UMI_STATUS_OK);
        UmiMediaImageEdit oversized = {0U, 0U, 4097U, 4096U, UMI_MEDIA_IMAGE_ORIGINAL};
        CHECK(UmiMediaImageSurfaceApplyEdit(large, &oversized, NULL, &result) ==
                  UMI_STATUS_CAPACITY_EXCEEDED &&
              result == NULL);
        umi_media_image_surface_destroy(large);
    }
    else
    {
        CHECK(UmiMediaImageSurfaceApplyEdit(strcmp(mode, "null-source") == 0 ? NULL : source,
                                            strcmp(mode, "null-edit") == 0 ? NULL : &edit, cancel,
                                            strcmp(mode, "null-output") == 0 ? NULL : &result) ==
              expected_status);
        if (expected_status == UMI_STATUS_OK)
        {
            UmiMediaImageSurfaceSnapshot info;
            CHECK(umi_media_image_surface_snapshot(result, &info) == UMI_STATUS_OK && info.width == width &&
                  info.height == height);
            for (size_t i = 0U; i < width * height; ++i)
            {
                UmiMediaRgbaPixel pixel;
                CHECK(umi_media_image_surface_get_pixel(result, i % width, i / width, &pixel) ==
                      UMI_STATUS_OK);
                CHECK(pixel.red == expected[i] && pixel.green == expected[i] + 10U &&
                      pixel.blue == expected[i] + 20U && pixel.alpha == expected[i] + 30U);
            }
        }
        else
            CHECK(result == NULL);
    }
    CHECK(umi_media_image_surface_snapshot(source, &after) == UMI_STATUS_OK &&
          after.revision == before.revision);
    for (size_t i = 0U; i < 6U; ++i)
    {
        UmiMediaRgbaPixel pixel;
        CHECK(umi_media_image_surface_get_pixel(source, i % 3U, i / 3U, &pixel) == UMI_STATUS_OK &&
              pixel.red == i + 1U);
    }
    umi_media_image_surface_destroy(source);
    umi_media_image_surface_destroy(result);
    umi_cancellation_token_destroy(cancel);
    return 0;
}

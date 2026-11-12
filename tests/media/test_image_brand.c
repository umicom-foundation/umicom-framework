/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_image_brand.c
 *
 * PURPOSE:
 *   Decode canonical SVG and exported PNG pixels while checking source and existing-file preservation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/ui/gtk4/image_brand.h"
#include "umicom/platform/input_file.h"
#include <string.h>
static bool EqualPixel(UmiMediaRgbaPixel a, UmiMediaRgbaPixel b)
{
    return a.red == b.red && a.green == b.green && a.blue == b.blue && a.alpha == b.alpha;
}
static void SameImage(const UmiMediaImageSurface *a, const UmiMediaImageSurface *b)
{
    UmiMediaImageSurfaceSnapshot left, right;
    CHECK(umi_media_image_surface_snapshot(a, &left) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_snapshot(b, &right) == UMI_STATUS_OK);
    CHECK(left.width == right.width && left.height == right.height);
    for (size_t y = 0U; y < left.height; ++y) {
        for (size_t x = 0U; x < left.width; ++x) {
            UmiMediaRgbaPixel p, q;
            CHECK(umi_media_image_surface_get_pixel(a, x, y, &p) == UMI_STATUS_OK);
            CHECK(umi_media_image_surface_get_pixel(b, x, y, &q) == UMI_STATUS_OK);
            CHECK(EqualPixel(p, q));
        }
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    size_t width = 512U, height = 256U;
    if (strcmp(mode, "portrait") == 0) { width = 256U; height = 512U; }
    if (strcmp(mode, "minimum") == 0) { width = 128U; height = 64U; }
    if (strcmp(mode, "small") == 0) { width = 127U; height = 64U; }
    UmiMediaRgbaPixel color = {0U, 0U, 255U, 255U};
    if (strcmp(mode, "transparent") == 0) color.alpha = 0U;
    if (strcmp(mode, "light") == 0) color = (UmiMediaRgbaPixel){255U,255U,255U,255U};
    UmiMediaImageSurface *source = NULL, *result = NULL;
    CHECK(umi_media_image_surface_create(width, height, &source) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_clear(source, color) == UMI_STATUS_OK);
    UmiMediaImageSurfaceSnapshot before;
    CHECK(umi_media_image_surface_snapshot(source, &before) == UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel") == 0) umi_cancellation_token_request(cancel);
    if (strcmp(mode, "occupied-output") == 0) result = source;
    UmiStatus expected = strcmp(mode, "cancel") == 0 ? UMI_STATUS_CANCELLED :
        strcmp(mode, "occupied-output") == 0 ? UMI_STATUS_INVALID_STATE :
        (strcmp(mode, "small") == 0 || strcmp(mode, "null-source") == 0 ||
         strcmp(mode, "null-output") == 0) ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
    CHECK(UmiGtk4ImageApplyBrand(strcmp(mode, "null-source") == 0 ? NULL : source, cancel,
        strcmp(mode, "null-output") == 0 ? NULL : &result) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(result == (strcmp(mode, "occupied-output") == 0 ? source : NULL));
    } else {
        CHECK(result != NULL && result != source);
        UmiMediaImageSurfaceSnapshot branded;
        CHECK(umi_media_image_surface_snapshot(result, &branded) == UMI_STATUS_OK);
        CHECK(branded.width == width && branded.height == height);
        size_t changed = 0U, red = 0U;
        for (size_t y = 0U; y < height; ++y) {
            for (size_t x = 0U; x < width; ++x) {
                UmiMediaRgbaPixel pixel, original;
                CHECK(umi_media_image_surface_get_pixel(source, x, y, &original) == UMI_STATUS_OK);
                CHECK(EqualPixel(original, color));
                CHECK(umi_media_image_surface_get_pixel(result, x, y, &pixel) == UMI_STATUS_OK);
                if (x < width * 3U / 4U || y < height * 3U / 4U) CHECK(EqualPixel(pixel, color));
                if (!EqualPixel(pixel, color)) ++changed;
                if (pixel.red > 80U && pixel.red > pixel.green * 2U && pixel.red > pixel.blue * 2U) ++red;
            }
        }
        CHECK(changed > 0U && red > 0U);
        if (strcmp(mode, "png") == 0 || strcmp(mode, "existing") == 0 ||
            strcmp(mode, "relative-output") == 0 || strcmp(mode, "null-receipt") == 0) {
            if (!UmiMediaPngAvailable() || !UmiMediaPngEncoderAvailable()) {
                umi_media_image_surface_destroy(result); umi_media_image_surface_destroy(source);
                umi_cancellation_token_destroy(cancel); return 77;
            }
            char directory[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY];
            FixtureDirectory(directory); FixturePath(output, directory, "branded.png");
            static const char sentinel[] = "Keep existing bytes";
            if (strcmp(mode, "existing") == 0) {
                UmiOutputFile *file = NULL;
                CHECK(UmiOutputFileCreate(output, &file) == UMI_STATUS_OK);
                CHECK(UmiOutputFileWrite(file, sentinel, sizeof(sentinel)) == UMI_STATUS_OK);
                CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
            }
            UmiMediaPngWriteResult written;
            memset(&written, 0x51, sizeof(written));
            UmiStatus status = UmiGtk4ImageWriteBrandedPng(source,
                strcmp(mode, "relative-output") == 0 ? "relative.png" : output, cancel,
                strcmp(mode, "null-receipt") == 0 ? NULL : &written);
            if (strcmp(mode, "null-receipt") == 0) CHECK(status == UMI_STATUS_INVALID_ARGUMENT);
            else if (strcmp(mode, "relative-output") == 0) {
                CHECK(status == UMI_STATUS_INVALID_ARGUMENT && !written.created);
            } else if (strcmp(mode, "existing") == 0) {
                CHECK(status == UMI_STATUS_ALREADY_EXISTS && !written.created);
                size_t size = 0U; unsigned char *bytes = FixtureRead(output, &size);
                CHECK(size == sizeof(sentinel) && memcmp(bytes, sentinel, size) == 0); free(bytes);
            } else {
                CHECK(status == UMI_STATUS_OK && written.created);
                unsigned char *bytes = NULL; size_t size = 0U;
                CHECK(UmiInputFileRead(output, UMI_MEDIA_PNG_INPUT_LIMIT, &bytes, &size) == UMI_STATUS_OK);
                UmiMediaImageSurface *decoded = NULL;
                CHECK(UmiMediaPngDecode(bytes, size, NULL, &decoded) == UMI_STATUS_OK);
                SameImage(result, decoded);
                umi_media_image_surface_destroy(decoded); UmiInputFileFree(bytes);
            }
        } else CHECK(strcmp(mode, "apply") == 0 || strcmp(mode, "transparent") == 0 ||
                     strcmp(mode, "light") == 0 || strcmp(mode, "portrait") == 0 ||
                     strcmp(mode, "minimum") == 0);
    }
    UmiMediaImageSurfaceSnapshot after;
    CHECK(umi_media_image_surface_snapshot(source, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision && after.width == before.width && after.height == before.height);
    if (result != source) umi_media_image_surface_destroy(result);
    umi_media_image_surface_destroy(source);
    umi_cancellation_token_destroy(cancel);
    return 0;
}

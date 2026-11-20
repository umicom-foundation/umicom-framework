/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/creative_assets/test_edited_png_export.c
 *
 * PURPOSE:
 *   Decode an edited asset export and verify final-coordinate branding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../build_log/fixture.h"
#include "../../adapters/gtk4/creative_asset_png_private.h"
#include "umicom/platform/input_file.h"
#include <string.h>

static bool SamePixel(UmiMediaRgbaPixel a, UmiMediaRgbaPixel b)
{
    return a.red == b.red && a.green == b.green && a.blue == b.blue && a.alpha == b.alpha;
}

static void CheckExport(const UmiMediaImageSurface *edited, const char *path, bool branded)
{
    unsigned char *bytes = NULL;
    size_t size = 0U;
    CHECK(UmiInputFileRead(path, UMI_MEDIA_PNG_INPUT_LIMIT, &bytes, &size) == UMI_STATUS_OK);
    UmiMediaImageSurface *saved = NULL;
    CHECK(UmiMediaPngDecode(bytes, size, NULL, &saved) == UMI_STATUS_OK);
    free(bytes);
    UmiMediaImageSurfaceSnapshot info;
    CHECK(umi_media_image_surface_snapshot(saved, &info) == UMI_STATUS_OK);
    CHECK(info.width == 256U && info.height == 512U);
    size_t visibleAccent = 0U;
    for (size_t y = 0U; y < info.height; ++y) {
        for (size_t x = 0U; x < info.width; ++x) {
            UmiMediaRgbaPixel expected, actual;
            CHECK(umi_media_image_surface_get_pixel(edited, x, y, &expected) == UMI_STATUS_OK);
            CHECK(umi_media_image_surface_get_pixel(saved, x, y, &actual) == UMI_STATUS_OK);
            if (!branded || x < 192U || y < 384U) CHECK(SamePixel(expected, actual));
            if (!SamePixel(expected, actual) && actual.red > 80U &&
                actual.red > 2U * actual.green && actual.red > 2U * actual.blue) ++visibleAccent;
        }
    }
    CHECK(branded ? visibleAccent > 0U : visibleAccent == 0U);
    umi_media_image_surface_destroy(saved);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    const bool plain = strcmp(mode, "plain") == 0;
    const bool small = strcmp(mode, "too-small") == 0;
    const bool cancelled = strcmp(mode, "cancel-before-export") == 0;
    const bool existing = strcmp(mode, "existing") == 0;
    const bool invalid = strcmp(mode, "invalid-crop") == 0;
    if (!plain && !small && !cancelled && !existing && !invalid && strcmp(mode, "branded") != 0)
        return 2;
    if (!UmiMediaPngAvailable() || !UmiMediaPngEncoderAvailable()) return 77;

    const UmiMediaRgbaPixel marker = {35U, 121U, 70U, 255U};
    UmiMediaImageSurface *source = NULL, *edited = NULL;
    CHECK(umi_media_image_surface_create(528U, 264U, &source) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_clear(source, (UmiMediaRgbaPixel){0U, 0U, 255U, 255U}) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_set_pixel(source, 16U, 263U, marker) == UMI_STATUS_OK);
    unsigned char *fixture = NULL;
    size_t fixtureSize = 0U;
    CHECK(UmiMediaPngEncode(source, NULL, &fixture, &fixtureSize) == UMI_STATUS_OK);
    UmiMediaImageEdit edit = {invalid ? 528U : 16U, 8U, 0U, small ? 127U : 0U, UMI_MEDIA_IMAGE_CLOCKWISE};
    UmiStatus status = UmiCreativePrepareEditedPng(fixture, fixtureSize, &edit, NULL, &edited);
    UmiMediaPngFree(fixture);
    if (invalid) {
        CHECK(status == UMI_STATUS_INVALID_ARGUMENT && edited == NULL);
        umi_media_image_surface_destroy(source);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK);
    if (!small) {
        UmiMediaRgbaPixel pixel;
        CHECK(umi_media_image_surface_get_pixel(edited, 0U, 0U, &pixel) == UMI_STATUS_OK);
        CHECK(SamePixel(pixel, marker));
    }

    char directory[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(directory);
    FixturePath(path, directory, "edited.png");
    const char sentinel[] = "Existing asset must remain unchanged.";
    if (existing) {
        UmiOutputFile *file = NULL;
        CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(file, sentinel, sizeof(sentinel)) == UMI_STATUS_OK);
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
        UmiOutputFileDestroy(file);
    }
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (cancelled) umi_cancellation_token_request(cancel);
    UmiMediaPngWriteResult result;
    status = UmiCreativeExportPreparedPng(edited, !plain, path, cancel, &result);
    if (existing) {
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && !result.created);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(path, &size);
        CHECK(size == sizeof(sentinel) && memcmp(bytes, sentinel, size) == 0);
        free(bytes);
    } else if (small || cancelled) {
        CHECK(status == (small ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_CANCELLED));
        CHECK(!result.created);
        unsigned char *bytes = NULL;
        size_t size = 0U;
        CHECK(UmiInputFileRead(path, UMI_MEDIA_PNG_INPUT_LIMIT, &bytes, &size) == UMI_STATUS_NOT_FOUND);
    } else {
        CHECK(status == UMI_STATUS_OK && result.created && result.file.bytes_written != 0U);
        CheckExport(edited, path, !plain);
    }
    UmiMediaRgbaPixel original;
    CHECK(umi_media_image_surface_get_pixel(source, 16U, 263U, &original) == UMI_STATUS_OK);
    CHECK(SamePixel(original, marker));
    umi_cancellation_token_destroy(cancel);
    umi_media_image_surface_destroy(edited);
    umi_media_image_surface_destroy(source);
    return 0;
}

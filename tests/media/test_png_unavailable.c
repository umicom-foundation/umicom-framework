/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_png_unavailable.c
 *
 * PURPOSE:
 *   Verify unavailable PNG admission preserves images, outputs and existing files.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media/png_image.h"
#include "../build_log/fixture.h"

int main(int argc, char **argv) {
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *cases[] = {"availability", "decode", "decode-cancelled", "decode-null-input", "decode-null-output",
        "decode-live-output", "decode-limit", "encode", "encode-cancelled", "encode-null-input", "encode-null-output",
        "encode-null-size", "encode-live-output", "write", "write-cancelled", "write-existing", "write-relative", "write-null-result"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i) known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U) return 2;
    CHECK(!UmiMediaPngAvailable() && !UmiMediaPngEncoderAvailable());
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    UmiMediaImageSurface *image = NULL;
    CHECK(umi_media_image_surface_create(2U, 1U, &image) == UMI_STATUS_OK);
    const unsigned char row[] = {17U, 45U, 90U, 255U, 12U, 34U, 56U, 78U};
    CHECK(UmiMediaImageSurfaceWriteRgbaRow(image, 0U, row, sizeof row) == UMI_STATUS_OK);
    UmiMediaImageSurfaceSnapshot before, after;
    CHECK(umi_media_image_surface_snapshot(image, &before) == UMI_STATUS_OK);
    UmiStatus status;
    if (strstr(mode, "cancelled") != NULL) umi_cancellation_token_request(cancel);
    if (!strcmp(mode, "availability")) {
        CHECK(!UmiMediaPngAvailable() && !UmiMediaPngEncoderAvailable());
    } else if (!strncmp(mode, "decode", 6U)) {
        UmiMediaImageSurface *out = !strcmp(mode, "decode-live-output") ? image : NULL;
        status = UmiMediaPngDecode(!strcmp(mode, "decode-null-input") ? NULL : row,
            !strcmp(mode, "decode-limit") ? UMI_MEDIA_PNG_INPUT_LIMIT + 1U : sizeof row,
            cancel, !strcmp(mode, "decode-null-output") ? NULL : &out);
        UmiStatus expected = !strcmp(mode, "decode-cancelled") ? UMI_STATUS_CANCELLED :
            !strcmp(mode, "decode-null-input") || !strcmp(mode, "decode-null-output") ? UMI_STATUS_INVALID_ARGUMENT :
            !strcmp(mode, "decode-live-output") ? UMI_STATUS_INVALID_STATE :
            !strcmp(mode, "decode-limit") ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_NOT_IMPLEMENTED;
        CHECK(status == expected);
        CHECK(out == (!strcmp(mode, "decode-live-output") ? image : NULL));
    } else if (!strncmp(mode, "encode", 6U)) {
        unsigned char sentinel = 77U;
        unsigned char *out = !strcmp(mode, "encode-live-output") ? &sentinel : NULL;
        size_t size = 12345U;
        status = UmiMediaPngEncode(!strcmp(mode, "encode-null-input") ? NULL : image, cancel,
            !strcmp(mode, "encode-null-output") ? NULL : &out,
            !strcmp(mode, "encode-null-size") ? NULL : &size);
        UmiStatus expected = !strcmp(mode, "encode-cancelled") ? UMI_STATUS_CANCELLED :
            !strcmp(mode, "encode-null-input") || !strcmp(mode, "encode-null-output") || !strcmp(mode, "encode-null-size")
                ? UMI_STATUS_INVALID_ARGUMENT :
            !strcmp(mode, "encode-live-output") ? UMI_STATUS_INVALID_STATE : UMI_STATUS_NOT_IMPLEMENTED;
        CHECK(status == expected && size == 12345U && sentinel == 77U);
        CHECK(out == (!strcmp(mode, "encode-live-output") ? &sentinel : NULL));
    } else if (!strncmp(mode, "write", 5U)) {
        char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
        FixtureDirectory(root); FixturePath(path, root, "image.png");
        if (!strcmp(mode, "write-existing")) {
            UmiOutputFile *file = NULL; CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
            CHECK(UmiOutputFileWrite(file, row, sizeof row) == UMI_STATUS_OK);
            CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
        }
        UmiMediaPngWriteResult result; memset(&result, 0xA5, sizeof result);
        status = UmiMediaPngWriteNew(image, !strcmp(mode, "write-relative") ? "image.png" : path, cancel,
            !strcmp(mode, "write-null-result") ? NULL : &result);
        UmiStatus expected = !strcmp(mode, "write-relative") || !strcmp(mode, "write-null-result") ? UMI_STATUS_INVALID_ARGUMENT :
            !strcmp(mode, "write-cancelled") ? UMI_STATUS_CANCELLED : UMI_STATUS_NOT_IMPLEMENTED;
        CHECK(status == expected);
        if (strcmp(mode, "write-null-result"))
            CHECK(!result.created && result.file.bytes_written == 0U && result.file.path[0] == '\0');
        if (!strcmp(mode, "write-existing")) {
            size_t size = 0U; unsigned char *actual = FixtureRead(path, &size);
            CHECK(size == sizeof row && memcmp(actual, row, size) == 0); free(actual);
        } else if (strcmp(mode, "write-relative")) {
            /* Exclusive creation proves no file was created by the refused codec. */
            UmiOutputFile *file = NULL; CHECK(UmiOutputFileCreate(path, &file) == UMI_STATUS_OK);
            CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK); UmiOutputFileDestroy(file);
        }
    } else return 2;
    CHECK(umi_media_image_surface_snapshot(image, &after) == UMI_STATUS_OK);
    CHECK(after.width == before.width && after.height == before.height && after.revision == before.revision);
    UmiMediaRgbaPixel pixels[2];
    CHECK(umi_media_image_surface_read_row(image, 0U, pixels, 2U) == UMI_STATUS_OK);
    CHECK(pixels[0].red == 17U && pixels[0].green == 45U && pixels[1].blue == 56U && pixels[1].alpha == 78U);
    umi_media_image_surface_destroy(image); umi_cancellation_token_destroy(cancel);
    return 0;
}

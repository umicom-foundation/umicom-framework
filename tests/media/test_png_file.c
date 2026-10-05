/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_png_file.c
 * PURPOSE: Check exclusive native PNG output and write receipts without modifying source pixels or existing files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif
#include "../build_log/fixture.h"
#include "umicom/media/png_image.h"
#include "umicom/platform/rooted_files.h"
#include "umicom/platform/filesystem.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *cases[] = {"write",    "unicode-path",   "existing",     "cancelled",
                                            "relative", "missing-parent", "null-surface", "null-result"};
    size_t known = 0U;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        known += (size_t)(strcmp(mode, cases[i]) == 0);
    if (known != 1U)
        return 2;
    if (!UmiMediaPngAvailable() || !UmiMediaPngEncoderAvailable())
        return 77;
    char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    FixtureDirectory(root);
    const char *leaf = strcmp(mode, "unicode-path") == 0     ? "caf\xc3\xa9.png"
                       : strcmp(mode, "missing-parent") == 0 ? "absent/image.png"
                                                             : "image.png";
    FixturePath(path, root, leaf);
    UmiMediaImageSurface *image = NULL, *decoded = NULL;
    CHECK(umi_media_image_surface_create(1U, 1U, &image) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_clear(image, (UmiMediaRgbaPixel){40U, 80U, 120U, 160U}) == UMI_STATUS_OK);
    UmiCancellationToken *cancel = NULL;
    CHECK(umi_cancellation_token_create(&cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancelled") == 0)
        umi_cancellation_token_request(cancel);
    if (strcmp(mode, "existing") == 0)
        CHECK(UmiRootedFileWrite(root, leaf, "keep", 4U) == UMI_STATUS_OK);
    UmiMediaPngWriteResult result;
    UmiStatus status = UmiMediaPngWriteNew(strcmp(mode, "null-surface") == 0 ? NULL : image,
                                           strcmp(mode, "relative") == 0 ? "relative.png" : path, cancel,
                                           strcmp(mode, "null-result") == 0 ? NULL : &result);
    if (strcmp(mode, "write") == 0 || strcmp(mode, "unicode-path") == 0)
    {
        CHECK(status == UMI_STATUS_OK && result.created && result.file.closed &&
              result.file.status == UMI_STATUS_OK);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(path, &size);
        CHECK(size == result.file.bytes_written &&
              UmiMediaPngDecode(bytes, size, NULL, &decoded) == UMI_STATUS_OK);
        free(bytes);
        UmiMediaRgbaPixel pixel;
        CHECK(umi_media_image_surface_get_pixel(decoded, 0U, 0U, &pixel) == UMI_STATUS_OK &&
              pixel.red == 40U && pixel.green == 80U && pixel.blue == 120U && pixel.alpha == 160U);
    }
    else if (strcmp(mode, "existing") == 0)
    {
        CHECK(status == UMI_STATUS_ALREADY_EXISTS && !result.created);
        size_t size = 0U;
        unsigned char *bytes = FixtureRead(path, &size);
        CHECK(size == 4U && memcmp(bytes, "keep", 4U) == 0);
        free(bytes);
    }
    else
    {
        CHECK(status != UMI_STATUS_OK && !umi_fs_exists(path));
        if (strcmp(mode, "null-result") != 0)
            CHECK(!result.created);
        if (strcmp(mode, "cancelled") == 0)
            CHECK(status == UMI_STATUS_CANCELLED);
    }
    umi_media_image_surface_destroy(image);
    umi_media_image_surface_destroy(decoded);
    umi_cancellation_token_destroy(cancel);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_image_brand_failure.c
 *
 * PURPOSE:
 *   Verify SVG decode failures, invalid raster metadata and late cancellation preserve the original and create no substitute output.
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
#include "../../adapters/gtk4/brand_widget.h"
#include <string.h>

static const char *Scenario;
static UmiCancellationToken *Cancellation;
static GdkPixbuf *FixtureDecode(gboolean wordmark, gboolean dark,
                               int height, int scale, GError **error)
{
    (void)wordmark; (void)dark; (void)height; (void)scale;
    if (strcmp(Scenario, "decode") == 0 || strcmp(Scenario, "save-decode") == 0) {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_FAILED, "Fixture SVG loader failure");
        return NULL;
    }
    if (strcmp(Scenario, "cancel-after-decode") == 0)
        umi_cancellation_token_request(Cancellation);
    int width = strcmp(Scenario, "width") == 0 ? 1065 : 24;
    int rows = strcmp(Scenario, "height") == 0 ? 257 : 6;
    GdkPixbuf *pixels = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, rows);
    CHECK(pixels != NULL);
    gdk_pixbuf_fill(pixels, 0xff0000ffU);
    return pixels;
}
static const guchar *FixturePixels(const GdkPixbuf *pixels)
{
    return strcmp(Scenario, "pixels") == 0 ? NULL : gdk_pixbuf_read_pixels(pixels);
}
/* Only the decoder boundary is replaced. Compile the real export adapter under
 * fixture names to prove failures do not publish an unbranded surface or file.
 * The production symbols remain linked for other native tests. */
UmiStatus UmiFixtureImageApplyBrand(const UmiMediaImageSurface *, const UmiCancellationToken *,
                                  UmiMediaImageSurface **);
UmiStatus UmiFixtureImageWriteBrandedPng(const UmiMediaImageSurface *, const char *,
                                      const UmiCancellationToken *, UmiMediaPngWriteResult *);
#define UmiGtkBrandDecode FixtureDecode
#define gdk_pixbuf_read_pixels FixturePixels
#define UmiGtk4ImageApplyBrand UmiFixtureImageApplyBrand
#define UmiGtk4ImageWriteBrandedPng UmiFixtureImageWriteBrandedPng
#include "../../adapters/gtk4/image_brand_gtk4.c"
#undef UmiGtk4ImageWriteBrandedPng
#undef UmiGtk4ImageApplyBrand
#undef gdk_pixbuf_read_pixels
#undef UmiGtkBrandDecode

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Scenario = argv[1];
    if (strcmp(Scenario, "decode") != 0 && strcmp(Scenario, "save-decode") != 0 &&
        strcmp(Scenario, "width") != 0 && strcmp(Scenario, "height") != 0 &&
        strcmp(Scenario, "pixels") != 0 && strcmp(Scenario, "cancel-after-decode") != 0) return 2;
    UmiMediaImageSurface *source = NULL, *result = NULL;
    CHECK(umi_media_image_surface_create(128U, 64U, &source) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_clear(source, (UmiMediaRgbaPixel){1U,2U,3U,255U}) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&Cancellation) == UMI_STATUS_OK);
    UmiMediaImageSurfaceSnapshot before, after;
    CHECK(umi_media_image_surface_snapshot(source, &before) == UMI_STATUS_OK);
    UmiStatus expected = strcmp(Scenario, "decode") == 0 || strcmp(Scenario, "save-decode") == 0 ?
        UMI_STATUS_IO_ERROR : strcmp(Scenario, "cancel-after-decode") == 0 ?
        UMI_STATUS_CANCELLED : UMI_STATUS_INVALID_STATE;
    if (strcmp(Scenario, "save-decode") == 0) {
        char directory[UMI_PATH_CAPACITY], output[UMI_PATH_CAPACITY];
        FixtureDirectory(directory); FixturePath(output, directory, "refused.png");
        UmiMediaPngWriteResult receipt;
        memset(&receipt, 0x53, sizeof(receipt));
        CHECK(UmiFixtureImageWriteBrandedPng(source, output, Cancellation, &receipt) == expected);
        CHECK(!receipt.created);
        unsigned char *bytes = NULL; size_t size = 0U;
        CHECK(UmiInputFileRead(output, 1024U, &bytes, &size) == UMI_STATUS_NOT_FOUND);
        CHECK(bytes == NULL && size == 0U);
    } else {
        CHECK(UmiFixtureImageApplyBrand(source, Cancellation, &result) == expected);
        CHECK(result == NULL);
    }
    CHECK(umi_media_image_surface_snapshot(source, &after) == UMI_STATUS_OK);
    CHECK(after.revision == before.revision);
    for (size_t y = 0U; y < 64U; ++y) {
        for (size_t x = 0U; x < 128U; ++x) {
            UmiMediaRgbaPixel pixel;
            CHECK(umi_media_image_surface_get_pixel(source, x, y, &pixel) == UMI_STATUS_OK);
            CHECK(pixel.red == 1U && pixel.green == 2U && pixel.blue == 3U && pixel.alpha == 255U);
        }
    }
    umi_cancellation_token_destroy(Cancellation);
    umi_media_image_surface_destroy(source);
    return 0;
}

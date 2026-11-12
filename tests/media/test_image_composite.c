/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/media/test_image_composite.c
 *
 * PURPOSE:
 *   Verify exact alpha composition, immutable inputs, publication cancellation and revision exhaustion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/media/image_composite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; \
} } while (0)
static unsigned CancelAfter, CancelChecks;
static bool ObserveCancellation(const UmiCancellationToken *token)
{
    ++CancelChecks;
    return (CancelAfter != 0U && CancelChecks >= CancelAfter) ||
        umi_cancellation_token_is_requested(token);
}
/* Compile the owner into this boundary fixture to inject revision exhaustion
 * and deterministic cancellation without allocating enormous images or racing
 * a second thread. The production public API remains opaque. */
#define umi_cancellation_token_is_requested ObserveCancellation
#include "../../src/media/image_surface.c"
#undef umi_cancellation_token_is_requested

static bool Same(UmiMediaRgbaPixel a, UmiMediaRgbaPixel b)
{
    return a.red == b.red && a.green == b.green && a.blue == b.blue && a.alpha == b.alpha;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *mode = argv[1];
    UmiMediaImageSurface *base = NULL, *over = NULL, *result = NULL;
    CHECK(umi_media_image_surface_create(4U, 3U, &base) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_create(2U, 2U, &over) == UMI_STATUS_OK);
    UmiMediaRgbaPixel blue = {0U, 0U, 255U, 255U}, red = {255U, 0U, 0U, 128U};
    UmiMediaRgbaPixel expected = {128U, 0U, 127U, 255U};
    size_t x = 1U, y = 1U;
    UmiStatus outcome = UMI_STATUS_OK;
    bool revision = strncmp(mode, "revision-", 9U) == 0;
    if (strcmp(mode, "opaque") == 0) { red.alpha = 255U; expected = red; }
    else if (strcmp(mode, "transparent") == 0) { red.alpha = 0U; expected = blue; }
    else if (strcmp(mode, "over-clear") == 0) { blue.alpha = 0U; expected = red; }
    else if (strcmp(mode, "zero-alpha") == 0) { blue.alpha = red.alpha = 0U; expected = blue; }
    else if (strcmp(mode, "partial") == 0) { blue.alpha = 128U; expected = (UmiMediaRgbaPixel){170U, 0U, 85U, 192U}; }
    else if (strcmp(mode, "low-alpha") == 0) { blue.alpha = red.alpha = 1U; expected = (UmiMediaRgbaPixel){128U, 0U, 127U, 2U}; }
    else if (strcmp(mode, "rounded") == 0) { red.alpha = 127U; expected = (UmiMediaRgbaPixel){127U, 0U, 128U, 255U}; }
    else if (strcmp(mode, "alias") == 0) { blue.alpha = 128U; }
    else if (strcmp(mode, "last-corner") == 0) { x = 2U; y = 1U; }
    else if (strcmp(mode, "origin") == 0) { x = y = 0U; }
    else if (strcmp(mode, "x-out") == 0) { x = 4U; outcome = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "y-out") == 0) { y = 3U; outcome = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "width-out") == 0) { x = 3U; outcome = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "height-out") == 0) { y = 2U; outcome = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "x-overflow") == 0) { x = SIZE_MAX; outcome = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "y-overflow") == 0) { y = SIZE_MAX; outcome = UMI_STATUS_INVALID_ARGUMENT; }
    else if (strcmp(mode, "cancel-before") == 0) { CancelAfter = 1U; outcome = UMI_STATUS_CANCELLED; }
    else if (strcmp(mode, "cancel-copy") == 0) { CancelAfter = 3U; outcome = UMI_STATUS_CANCELLED; }
    else if (strcmp(mode, "cancel-composite") == 0) { CancelAfter = 6U; outcome = UMI_STATUS_CANCELLED; }
    else if (strcmp(mode, "cancel-publication") == 0) { CancelAfter = 8U; outcome = UMI_STATUS_CANCELLED; }
    else if (strcmp(mode, "over-limit") == 0) outcome = UMI_STATUS_CAPACITY_EXCEEDED;
    else if (!revision && strcmp(mode, "source-over") != 0 && strcmp(mode, "alias") != 0 &&
             strcmp(mode, "null-source") != 0 && strcmp(mode, "null-overlay") != 0 &&
             strcmp(mode, "null-output") != 0 && strcmp(mode, "occupied-output") != 0 &&
             strcmp(mode, "mixed") != 0 && strcmp(mode, "row-preservation") != 0)
        return 2;
    CHECK(umi_media_image_surface_clear(base, blue) == UMI_STATUS_OK);
    CHECK(umi_media_image_surface_clear(over, red) == UMI_STATUS_OK);
    if (revision) {
        bool last = strstr(mode, "last-") != NULL;
        base->revision = last ? UINT64_MAX - 1U : UINT64_MAX;
        uint64_t before = base->revision;
        UmiStatus status;
        if (strstr(mode, "clear")) status = umi_media_image_surface_clear(base, red);
        else if (strstr(mode, "pixel")) status = umi_media_image_surface_set_pixel(base, 1U, 1U, red);
        else if (strcmp(mode, "revision-row") == 0) {
            const unsigned char row[16] = {255U,0U,0U,128U, 255U,0U,0U,128U,
                                          255U,0U,0U,128U, 255U,0U,0U,128U};
            status = UmiMediaImageSurfaceWriteRgbaRow(base, 1U, row, sizeof(row));
        } else return 2;
        CHECK(status == (last ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED));
        CHECK(base->revision == (last ? UINT64_MAX : before));
        CHECK(Same(base->pixels[5], last ? red : blue));
        if (last) {
            CHECK(umi_media_image_surface_set_pixel(base, 1U, 1U, blue) == UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(Same(base->pixels[5], red) && base->revision == UINT64_MAX);
        }
    } else {
        if (strcmp(mode, "mixed") == 0) {
            CHECK(umi_media_image_surface_set_pixel(over, 1U, 0U,
                (UmiMediaRgbaPixel){0U,255U,0U,255U}) == UMI_STATUS_OK);
            CHECK(umi_media_image_surface_set_pixel(over, 0U, 1U,
                (UmiMediaRgbaPixel){255U,255U,255U,0U}) == UMI_STATUS_OK);
        }
        if (strcmp(mode, "row-preservation") == 0)
            CHECK(umi_media_image_surface_set_pixel(base, 0U, 0U, (UmiMediaRgbaPixel){1U,2U,3U,4U}) == UMI_STATUS_OK);
        UmiMediaRgbaPixel savedBase[12], savedOver[4];
        memcpy(savedBase, base->pixels, sizeof(savedBase));
        memcpy(savedOver, over->pixels, sizeof(savedOver));
        uint64_t baseRevision = base->revision, overRevision = over->revision;
        const UmiMediaImageSurface *input = base, *layer = over;
        UmiMediaImageSurface **output = &result;
        if (strcmp(mode, "null-source") == 0) { input = NULL; outcome = UMI_STATUS_INVALID_ARGUMENT; }
        if (strcmp(mode, "null-overlay") == 0) { layer = NULL; outcome = UMI_STATUS_INVALID_ARGUMENT; }
        if (strcmp(mode, "null-output") == 0) { output = NULL; outcome = UMI_STATUS_INVALID_ARGUMENT; }
        if (strcmp(mode, "occupied-output") == 0) { result = base; outcome = UMI_STATUS_INVALID_STATE; }
        if (strcmp(mode, "alias") == 0) { layer = base; x = y = 0U; }
        if (strcmp(mode, "over-limit") == 0) base->pixel_count = UMI_MEDIA_IMAGE_EDIT_MAX_PIXELS + 1U;
        CHECK(UmiMediaImageSurfaceComposite(input, layer, x, y, NULL, output) == outcome);
        base->pixel_count = 12U;
        CHECK(memcmp(savedBase, base->pixels, sizeof(savedBase)) == 0);
        CHECK(memcmp(savedOver, over->pixels, sizeof(savedOver)) == 0);
        CHECK(base->revision == baseRevision && over->revision == overRevision);
        if (outcome == UMI_STATUS_OK) {
            CHECK(result != base && result != over && result->width == 4U && result->height == 3U);
            for (size_t row = 0U; row < 3U; ++row) {
                for (size_t column = 0U; column < 4U; ++column) {
                    UmiMediaRgbaPixel want = savedBase[row * 4U + column];
                    if (strcmp(mode, "alias") == 0) want.alpha = 192U;
                    if (strcmp(mode, "alias") != 0 &&
                        column >= x && column - x < 2U && row >= y && row - y < 2U) {
                        want = expected;
                        if (strcmp(mode, "mixed") == 0 && column == x + 1U && row == y)
                            want = (UmiMediaRgbaPixel){0U,255U,0U,255U};
                        if (strcmp(mode, "mixed") == 0 && column == x && row == y + 1U) want = blue;
                    }
                    CHECK(Same(result->pixels[row * 4U + column], want));
                }
            }
        } else {
            CHECK(result == (strcmp(mode, "occupied-output") == 0 ? base : NULL));
        }
    }
    if (result != base) umi_media_image_surface_destroy(result);
    umi_media_image_surface_destroy(over);
    umi_media_image_surface_destroy(base);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_pixbuf_texture_gtk4.c
 *
 * PURPOSE:
 *   Check RGB/RGBA conversion, short final rows and texture ownership without opening a window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/pixbuf_texture_private.h"
#include <stdint.h>
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    GError *error = NULL;
    if (strcmp(argv[1], "null") == 0) {
        CHECK(UmiGtkTextureFromPixbuf(NULL, &error) == NULL);
        CHECK(error != NULL && error->domain == G_IO_ERROR && error->code == G_IO_ERROR_INVALID_ARGUMENT);
        g_clear_error(&error); return 0;
    }
    gboolean alpha = strcmp(argv[1], "rgba") == 0 || strcmp(argv[1], "transparent") == 0;
    gboolean transparent = strcmp(argv[1], "transparent") == 0;
    gboolean padded = strcmp(argv[1], "padded") == 0;
    gboolean released = strcmp(argv[1], "ownership") == 0;
    if (!alpha && !padded && !released && strcmp(argv[1], "rgb") != 0) return 2;
    int channels = alpha ? 4 : 3, stride = padded ? 8 : channels * 2;
    /* Allocate no last-row padding. Sanitizers can detect an accidental
     * stride*height read from this otherwise valid pixbuf. */
    gsize bytes = (gsize)stride + (gsize)channels * 2U;
    guchar *pixels = g_malloc0(bytes);
    for (int row = 0; row < 2; ++row) for (int column = 0; column < 2; ++column) {
        gsize offset = (gsize)row * (gsize)stride + (gsize)column * (gsize)channels;
        pixels[offset] = 120; pixels[offset + 1U] = 60; pixels[offset + 2U] = 20;
        if (alpha) pixels[offset + 3U] = transparent ? 0 : 255;
    }
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(pixels, GDK_COLORSPACE_RGB, alpha, 8, 2, 2, stride, NULL, NULL);
    CHECK(pixbuf != NULL);
    GdkTexture *texture = UmiGtkTextureFromPixbuf(pixbuf, &error);
    CHECK(texture != NULL && error == NULL);
    if (released) memset(pixels, 0xEE, bytes);
    g_object_unref(pixbuf); g_free(pixels);
    CHECK(gdk_texture_get_width(texture) == 2 && gdk_texture_get_height(texture) == 2);
    /* GDK_MEMORY_DEFAULT is native-endian premultiplied ARGB32. These opaque
     * and transparent fixtures avoid ambiguous fractional-alpha rounding. */
    uint32_t actual[4] = {0};
    gdk_texture_download(texture, (guchar *)actual, 2U * sizeof(uint32_t));
    for (size_t i = 0U; i < 4U; ++i) CHECK(actual[i] == (transparent ? 0U : UINT32_C(0xFF783C14)));
    g_object_unref(texture);
    return 0;
}

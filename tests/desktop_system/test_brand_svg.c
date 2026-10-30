/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_system/test_brand_svg.c
 *
 * PURPOSE:
 *   Verify canonical SVG transparency, display-scale resolution and bounded native branding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/brand_widget.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "Check failed at %d: %s\n", __LINE__, #c); return 1; } } while (0)
static int Decode(gboolean wordmark, gboolean dark, int scale)
{
    GError *error = NULL;
    GdkPixbuf *pixels = UmiGtkBrandDecode(wordmark, dark, 36, scale, &error);
    CHECK(error == NULL && pixels != NULL);
    CHECK(gdk_pixbuf_get_has_alpha(pixels));
    int width = gdk_pixbuf_get_width(pixels), height = gdk_pixbuf_get_height(pixels);
    CHECK(height >= 35 * scale && height <= 36 * scale);
    CHECK(width > 0 && width <= (wordmark ? 147 : 36) * scale);
    const guchar *data = gdk_pixbuf_read_pixels(pixels);
    int stride = gdk_pixbuf_get_rowstride(pixels), channels = gdk_pixbuf_get_n_channels(pixels);
    CHECK(channels == 4);
    size_t transparent = 0U, opaque = 0U;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            guchar alpha = data[(size_t)y * (size_t)stride + (size_t)x * 4U + 3U];
            transparent += alpha == 0U;
            opaque += alpha == 255U;
        }
    }
    /* An opaque rectangular backing and an entirely empty decode both fail. */
    CHECK(transparent > (size_t)width * (size_t)height / 10U && opaque > 0U);
    CHECK(data[3] == 0U);
    g_object_unref(pixels);
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "decode") == 0) {
        for (int mark = 0; mark < 2; ++mark)
            for (int dark = 0; dark < 2; ++dark)
                for (int scale = 1; scale <= 3; ++scale)
                    CHECK(Decode(mark, dark, scale) == 0);
        GError *error = NULL;
        CHECK(UmiGtkBrandDecode(TRUE, FALSE, 0, 1, &error) == NULL && error != NULL);
        g_clear_error(&error);
        CHECK(UmiGtkBrandDecode(TRUE, FALSE, 36, 9, &error) == NULL && error != NULL);
        g_clear_error(&error);
    } else if (strcmp(argv[1], "native") == 0) {
        if (!gtk_init_check()) return 77;
        GtkWidget *logo = UmiGtkBrandCreate(TRUE, 36);
        g_object_ref_sink(logo);
        CHECK(gtk_drawing_area_get_content_width(GTK_DRAWING_AREA(logo)) == 147);
        CHECK(gtk_drawing_area_get_content_height(GTK_DRAWING_AREA(logo)) == 36);
        CHECK(!gtk_widget_get_hexpand(logo) && !gtk_widget_get_vexpand(logo));
        UmiGtkBrandView view = {.wordmark = TRUE, .height = 36};
        cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 400, 100);
        cairo_t *cr = cairo_create(surface);
        UmiGtkBrandDraw(GTK_DRAWING_AREA(logo), cr, 0, 0, &view);
        CHECK(cairo_status(cr) == CAIRO_STATUS_SUCCESS);
        UmiGtkBrandDraw(GTK_DRAWING_AREA(logo), cr, 400, 100, &view);
        CHECK(view.pixels != NULL && cairo_status(cr) == CAIRO_STATUS_SUCCESS);
        cairo_surface_flush(surface);
        unsigned char *data = cairo_image_surface_get_data(surface);
        int stride = cairo_image_surface_get_stride(surface);
        /* A wide window must not expand the artwork beyond its logical width. */
        for (int y = 0; y < 100; ++y)
            for (int x = 160; x < 400; ++x)
                for (int byte = 0; byte < 4; ++byte)
                    CHECK(data[y * stride + x * 4 + byte] == 0U);
        g_clear_object(&view.pixels);
        cairo_destroy(cr);
        cairo_surface_destroy(surface);
        g_object_unref(logo);
    } else return 2;
    return 0;
}

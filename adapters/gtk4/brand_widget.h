/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/brand_widget.h
 *
 * PURPOSE:
 *   Render transparent canonical SVG artwork at display scale within bounded native headers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_GTK_BRAND_WIDGET_H
#define UMICOM_GTK_BRAND_WIDGET_H
#include <gtk/gtk.h>
#include "umicom_gtk_brand_resources.h"

typedef struct UmiGtkBrandView {
    gboolean wordmark;
    int height;
    int scale;
    gboolean dark;
    GdkPixbuf *pixels;
} UmiGtkBrandView;

static inline GdkPixbuf *UmiGtkBrandDecode(gboolean wordmark, gboolean dark,
                                         int height, int scale, GError **error)
{
    if (height < 1 || height > 256 || scale < 1 || scale > 8) {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
                            "Brand dimensions are outside the supported bounds.");
        return NULL;
    }
    const unsigned char *svg = wordmark
        ? (dark ? UmiBrandLogoDarkSvg : UmiBrandLogoSvg)
        : (dark ? UmiBrandIconDarkSvg : UmiBrandIconSvg);
    gsize length = wordmark
        ? (dark ? sizeof UmiBrandLogoDarkSvg : sizeof UmiBrandLogoSvg)
        : (dark ? sizeof UmiBrandIconDarkSvg : sizeof UmiBrandIconSvg);
    int width = wordmark ? (height * 1064 + 260) / 261 : height;
    GInputStream *input = g_memory_input_stream_new_from_data(svg, (gssize)length, NULL);
    /* Rasterise vector artwork only at the current monitor scale, preserving
     * alpha. No alternate bitmap or working-directory lookup is distributed. */
    GdkPixbuf *pixels = gdk_pixbuf_new_from_stream_at_scale(input,
        width * scale, height * scale, TRUE, NULL, error);
    g_object_unref(input);
    return pixels;
}

static inline void UmiGtkBrandRelease(gpointer data)
{
    UmiGtkBrandView *view = data;
    g_clear_object(&view->pixels);
    g_free(view);
}

static inline void UmiGtkBrandDraw(GtkDrawingArea *area, cairo_t *cr,
                                   int width, int height, gpointer data)
{
    UmiGtkBrandView *view = data;
    if (width <= 0 || height <= 0) return;
    GdkRGBA foreground;
#if GTK_CHECK_VERSION(4, 10, 0)
    gtk_widget_get_color(GTK_WIDGET(area), &foreground);
#else
    gtk_style_context_get_color(gtk_widget_get_style_context(GTK_WIDGET(area)), &foreground);
#endif
    gboolean dark = foreground.red + foreground.green + foreground.blue > 1.5;
    int scale = CLAMP(gtk_widget_get_scale_factor(GTK_WIDGET(area)), 1, 8);
    if (view->pixels == NULL || view->scale != scale || view->dark != dark) {
        GError *error = NULL;
        GdkPixbuf *next = UmiGtkBrandDecode(view->wordmark, dark, view->height, scale, &error);
        g_clear_object(&view->pixels);
        view->pixels = next;
        view->scale = scale;
        view->dark = dark;
        g_clear_error(&error);
    }
    if (view->pixels == NULL) return;
    /* Logical size stays constant on high-DPI screens. Never stretch a logo
     * across the window or let its physical pixel dimensions enlarge a header. */
    double imageWidth = (double)gdk_pixbuf_get_width(view->pixels) / scale;
    double imageHeight = (double)gdk_pixbuf_get_height(view->pixels) / scale;
    double fit = MIN(1.0, MIN((double)width / imageWidth, (double)height / imageHeight));
    cairo_save(cr);
    cairo_translate(cr, 0.0, ((double)height - imageHeight * fit) / 2.0);
    cairo_scale(cr, fit / scale, fit / scale);
    gdk_cairo_set_source_pixbuf(cr, view->pixels, 0.0, 0.0);
    cairo_paint(cr);
    cairo_restore(cr);
}

static inline GtkWidget *UmiGtkBrandCreate(gboolean wordmark, int height)
{
    height = CLAMP(height, 1, 256);
    GtkWidget *area = gtk_drawing_area_new();
    UmiGtkBrandView *view = g_new0(UmiGtkBrandView, 1);
    view->wordmark = wordmark;
    view->height = height;
    gtk_drawing_area_set_content_width(GTK_DRAWING_AREA(area),
        wordmark ? (height * 1064 + 260) / 261 : height);
    gtk_drawing_area_set_content_height(GTK_DRAWING_AREA(area), height);
    gtk_widget_set_halign(area, GTK_ALIGN_START);
    gtk_widget_set_valign(area, GTK_ALIGN_CENTER);
    gtk_widget_set_hexpand(area, FALSE);
    gtk_widget_set_vexpand(area, FALSE);
    gtk_widget_set_name(area, wordmark ? "umicom.brand.logo" : "umicom.brand.icon");
    gtk_accessible_update_property(GTK_ACCESSIBLE(area),
        GTK_ACCESSIBLE_PROPERTY_LABEL, wordmark ? "Umicom" : "Umicom icon", -1);
    gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(area), UmiGtkBrandDraw,
                                  view, UmiGtkBrandRelease);
    return area;
}
#endif

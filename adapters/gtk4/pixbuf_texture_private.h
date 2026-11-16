/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/pixbuf_texture_private.h
 *
 * PURPOSE:
 *   Create owned GTK textures from decoded RGB pixels without borrowing row padding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_GTK_PIXBUF_TEXTURE_PRIVATE_H
#define UMICOM_GTK_PIXBUF_TEXTURE_PRIVATE_H
#include <gtk/gtk.h>
#include <string.h>

/* Shared by branded surfaces. Pixbuf may omit the final row's padding, while
 * GdkMemoryTexture requires complete rows. Copy only pixel bytes into a checked
 * tightly packed allocation; GBytes then owns them independently of pixbuf.
 * This accepts ordinary 8-bit RGB/RGBA data, not arbitrary image encodings. */
static inline GdkTexture *UmiGtkTextureFromPixbuf(GdkPixbuf *pixbuf, GError **error)
{
    if (pixbuf == NULL || gdk_pixbuf_get_colorspace(pixbuf) != GDK_COLORSPACE_RGB ||
        gdk_pixbuf_get_bits_per_sample(pixbuf) != 8) {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_ARGUMENT,
            "Texture conversion requires an 8-bit RGB pixbuf.");
        return NULL;
    }
    int width = gdk_pixbuf_get_width(pixbuf), height = gdk_pixbuf_get_height(pixbuf);
    int channels = gdk_pixbuf_get_n_channels(pixbuf);
    int source_stride = gdk_pixbuf_get_rowstride(pixbuf);
    gboolean alpha = gdk_pixbuf_get_has_alpha(pixbuf);
    if (width <= 0 || height <= 0 || source_stride <= 0 ||
        channels != (alpha ? 4 : 3) || (gsize)width > G_MAXSIZE / (gsize)channels) {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
            "Invalid texture dimensions or pixel layout.");
        return NULL;
    }
    gsize stride = (gsize)width * (gsize)channels;
    if (stride > (gsize)source_stride || (gsize)height > G_MAXSIZE / stride ||
        (gsize)(height - 1) > (G_MAXSIZE - stride) / (gsize)source_stride) {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
            "Texture row sizes exceed the supported range.");
        return NULL;
    }
    const guchar *source = gdk_pixbuf_read_pixels(pixbuf);
    gsize required = (gsize)(height - 1) * (gsize)source_stride + stride;
    if (source == NULL || gdk_pixbuf_get_byte_length(pixbuf) < required) {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_INVALID_DATA,
            "Texture pixels do not cover the declared rows.");
        return NULL;
    }
    gsize size = stride * (gsize)height;
    guchar *packed = g_try_malloc(size);
    if (packed == NULL) {
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_NO_SPACE,
            "Unable to allocate texture pixels.");
        return NULL;
    }
    for (int row = 0; row < height; ++row)
        memcpy(packed + (gsize)row * stride,
            source + (gsize)row * (gsize)source_stride, stride);
    GBytes *bytes = g_bytes_new_take(packed, size);
    GdkTexture *texture = gdk_memory_texture_new(width, height,
        alpha ? GDK_MEMORY_R8G8B8A8 : GDK_MEMORY_R8G8B8, bytes, stride);
    g_bytes_unref(bytes);
    return texture;
}
#endif

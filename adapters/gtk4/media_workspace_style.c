/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/media_workspace_style.c
 *
 * PURPOSE:
 *   Apply Media-only styling and display canonical SVG branding independently of the working directory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "media_workspace_style.h"
#include "umicom_media_workspace_resources.h"

typedef struct MediaStyleOwner {
    GdkDisplay *display;
    GtkCssProvider *provider;
} MediaStyleOwner;

static void ReleaseStyle(gpointer data)
{
    MediaStyleOwner *owner = data;
    gtk_style_context_remove_provider_for_display(owner->display,
        GTK_STYLE_PROVIDER(owner->provider));
    g_object_unref(owner->provider);
    g_object_unref(owner->display);
    g_free(owner);
}

void UmiMediaWorkspaceStyle(GtkWidget *root)
{
    /* Scope every selector to this surface. Other products keep their own
     * palette, and closing this surface releases its display registration. */
    if (g_object_get_data(G_OBJECT(root), "umicom-media-style") != NULL) return;
    MediaStyleOwner *owner = g_new0(MediaStyleOwner, 1);
    owner->display = g_object_ref(gtk_widget_get_display(root));
    owner->provider = gtk_css_provider_new();
#if GTK_CHECK_VERSION(4, 12, 0)
    GBytes *css = g_bytes_new_static(MediaStyles, sizeof MediaStyles);
    gtk_css_provider_load_from_bytes(owner->provider, css);
    g_bytes_unref(css);
#else
    /* Retain older GTK support without using a deprecated loader on new GTK. */
    gtk_css_provider_load_from_data(owner->provider, (const char *)MediaStyles,
                                   (gssize)sizeof MediaStyles);
#endif
    gtk_style_context_add_provider_for_display(owner->display,
        GTK_STYLE_PROVIDER(owner->provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_set_data_full(G_OBJECT(root), "umicom-media-style", owner, ReleaseStyle);
    gtk_widget_add_css_class(root, "umicom-media-workspace");
}

static void RefreshBrand(GtkWidget *picture, GParamSpec *property, gpointer unused)
{
    (void)property;
    (void)unused;
    gboolean wordmark = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(picture), "media-wordmark"));
    const unsigned char *data = wordmark ? MediaLogoSvg : MediaIconSvg;
    gsize length = wordmark ? sizeof MediaLogoSvg : sizeof MediaIconSvg;
    int scale = gtk_widget_get_scale_factor(picture);
    int width = wordmark ? 132 : 28;
    int height = wordmark ? 34 : 28;
    GInputStream *input = g_memory_input_stream_new_from_data(data, (gssize)length, NULL);
    GError *error = NULL;
    /* Decode the SVG at the current display scale. The embedded input remains
     * vector artwork; no PNG copy or alternate symbol is stored in the product. */
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_stream_at_scale(input, width * scale,
        height * scale, TRUE, NULL, &error);
    g_object_unref(input);
    if (pixbuf != NULL) {
        GdkTexture *texture = gdk_texture_new_for_pixbuf(pixbuf);
        gtk_picture_set_paintable(GTK_PICTURE(picture), GDK_PAINTABLE(texture));
        g_object_unref(texture);
        g_object_unref(pixbuf);
    } else {
        gtk_widget_set_tooltip_text(picture, "Umicom SVG artwork could not be decoded.");
    }
    g_clear_error(&error);
}

GtkWidget *UmiMediaWorkspaceBrand(bool wordmark)
{
    GtkWidget *picture = gtk_picture_new();
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE);
    gtk_picture_set_content_fit(GTK_PICTURE(picture), GTK_CONTENT_FIT_CONTAIN);
    gtk_widget_set_size_request(picture, wordmark ? 132 : 28, wordmark ? 34 : 28);
    gtk_widget_set_valign(picture, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(picture, GTK_ALIGN_START);
    g_object_set_data(G_OBJECT(picture), "media-wordmark", GINT_TO_POINTER(wordmark));
    g_object_set_data(G_OBJECT(picture), "umicom-automation-id",
                      (gpointer)(wordmark ? "media.brand.logo" : "media.brand.icon"));
    gtk_accessible_update_property(GTK_ACCESSIBLE(picture),
        GTK_ACCESSIBLE_PROPERTY_LABEL, wordmark ? "Umicom" : "Umicom icon", -1);
    g_signal_connect(picture, "notify::scale-factor", G_CALLBACK(RefreshBrand), NULL);
    RefreshBrand(picture, NULL, NULL);
    return picture;
}

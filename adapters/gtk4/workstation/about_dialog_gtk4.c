/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/about_dialog_gtk4.c
 *
 * PURPOSE:
 *   Reuse canonical artwork and native window ownership for application About commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/about_dialog.h"
#include "umicom/ui/gtk4/workstation/shell_header.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include "../brand_widget.h"
#include "../pixbuf_texture_private.h"
#include <string.h>
#define ABOUT_CACHE_KEY "umicom-framework-about-window"
#define ABOUT_BUSY_KEY "umicom-framework-about-active"

/* Bound metadata before GTK copies it. This is a byte limit, not a character
 * count; the complete accepted value must also be valid UTF-8. */
static gboolean AboutTextValid(const char *text, size_t limit, gboolean empty)
{
    if (text == NULL) return FALSE;
    size_t bytes = 0U;
    while (bytes <= limit && text[bytes] != '\0') ++bytes;
    return bytes <= limit && (empty || bytes != 0U) &&
        g_utf8_validate(text, (gssize)bytes, NULL);
}
static UmiStatus AboutConfigValidate(const UmiGtk4AboutDialogConfig *config)
{
    if (config == NULL || umi_product_branding_validate(&config->branding) != UMI_STATUS_OK ||
        !AboutTextValid(config->branding.display_name, 255U, FALSE) ||
        !AboutTextValid(config->branding.vendor, 255U, FALSE) ||
        !AboutTextValid(config->branding.executable_name, 255U, FALSE) ||
        !AboutTextValid(config->version, 255U, FALSE) ||
        !AboutTextValid(config->comments, 4095U, TRUE)) return UMI_STATUS_INVALID_ARGUMENT;
    /* GTK's enum class avoids assuming a last value across GTK releases. */
    GEnumClass *licenses = g_type_class_ref(GTK_TYPE_LICENSE);
    gboolean valid = g_enum_get_value(licenses, (gint)config->license) != NULL;
    g_type_class_unref(licenses);
    return valid ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
static void AboutWeakRelease(gpointer data)
{
    GWeakRef *reference = data;
    g_weak_ref_clear(reference);
    g_free(reference);
}
static gboolean AboutConfigMatches(GtkWindow *window,
    const UmiGtk4AboutDialogConfig *config)
{
    GtkAboutDialog *dialog = GTK_ABOUT_DIALOG(window);
    const char *const *authors = gtk_about_dialog_get_authors(dialog);
    const char *executable = g_object_get_data(G_OBJECT(window), "umicom-about-executable");
    return !g_strcmp0(gtk_about_dialog_get_program_name(dialog), config->branding.display_name) &&
        !g_strcmp0(gtk_about_dialog_get_version(dialog), config->version) &&
        !g_strcmp0(gtk_about_dialog_get_comments(dialog), config->comments) &&
        authors != NULL && !g_strcmp0(authors[0], config->branding.vendor) &&
        authors[1] == NULL && !g_strcmp0(executable, config->branding.executable_name) &&
        gtk_about_dialog_get_license_type(dialog) == config->license;
}
UmiStatus UmiGtk4AboutDialogCreate(GtkWindow *parent,
    const UmiGtk4AboutDialogConfig *config, GtkWindow **out_window)
{
    if (out_window == NULL || (parent != NULL && !GTK_IS_WINDOW(parent)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (*out_window != NULL) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = AboutConfigValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (parent != NULL && !UmiGtk4WindowIsOpen(parent)) return UMI_STATUS_CANCELLED;
    if (parent != NULL) g_object_ref(parent);
    /* Copy before creating a toplevel: observers of GTK's window model may
     * synchronously release application-owned configuration. */
    char *name = g_strdup(config->branding.display_name);
    char *vendor = g_strdup(config->branding.vendor);
    char *executable = g_strdup(config->branding.executable_name);
    char *version = g_strdup(config->version);
    char *comments = g_strdup(config->comments);
    GtkLicense license = config->license;
    GtkWindow *window = GTK_WINDOW(gtk_about_dialog_new());
    g_object_ref(window);
    GtkAboutDialog *dialog = GTK_ABOUT_DIALOG(window);
    char *title = g_strdup_printf("About %s", name);
    char *copyright = g_strdup_printf("Copyright \302\251 %s", vendor);
    const char *authors[] = {vendor, NULL};
    gtk_window_set_title(window, title);
    gtk_about_dialog_set_program_name(dialog, name);
    gtk_about_dialog_set_version(dialog, version);
    gtk_about_dialog_set_comments(dialog, comments);
    gtk_about_dialog_set_copyright(dialog, copyright);
    gtk_about_dialog_set_authors(dialog, authors);
    gtk_about_dialog_set_license_type(dialog, license);
    g_object_set_data_full(G_OBJECT(window), "umicom-about-executable", executable, g_free);
    executable = NULL;
    gtk_window_set_hide_on_close(window, FALSE);
    gtk_widget_set_name(GTK_WIDGET(window), "umicom.about");
    if (parent != NULL) {
        gtk_window_set_transient_for(window, parent);
        gtk_window_set_destroy_with_parent(window, TRUE);
        gtk_window_set_modal(window, TRUE);
    }
    GdkRGBA foreground;
#if GTK_CHECK_VERSION(4, 10, 0)
    gtk_widget_get_color(GTK_WIDGET(window), &foreground);
#else
    gtk_style_context_get_color(gtk_widget_get_style_context(GTK_WIDGET(window)), &foreground);
#endif
    gboolean dark = foreground.red + foreground.green + foreground.blue > 1.5;
    GError *error = NULL;
    GdkPixbuf *pixels = UmiGtkBrandDecode(FALSE, dark, 96, 1, &error);
    if (pixels == NULL) status = UMI_STATUS_IO_ERROR;
    else {
/* GTK 4.20 deprecated the pixbuf constructor; use shared checked pixel packing while preserving canonical SVG artwork. The former implementation is retained for owner review. */
#if 0
        GdkTexture *texture = gdk_texture_new_for_pixbuf(pixels);
        gtk_about_dialog_set_logo(dialog, GDK_PAINTABLE(texture));
        g_object_unref(texture);
#endif
        GdkTexture *texture = UmiGtkTextureFromPixbuf(pixels, &error);
        if (texture == NULL) status = UMI_STATUS_IO_ERROR;
        else {
            gtk_about_dialog_set_logo(dialog, GDK_PAINTABLE(texture));
            g_object_unref(texture);
        }
        g_object_unref(pixels);
        /* Missing platform integration does not discard the embedded SVG.
         * Taskbar, DPI and installed icon appearance need native qualification. */
        (void)umi_gtk4_ws_apply_window_identity(window);
    }
    g_clear_error(&error);
    g_free(title); g_free(copyright); g_free(name); g_free(vendor);
    g_free(executable); g_free(version); g_free(comments);
    if (!UmiGtk4WindowIsOpen(window) || (parent != NULL &&
        (!UmiGtk4WindowIsOpen(parent) || gtk_window_get_transient_for(window) != parent)))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) *out_window = window;
    else { gtk_window_destroy(window); g_object_unref(window); }
    if (parent != NULL) g_object_unref(parent);
    return status;
}
UmiStatus UmiGtk4AboutDialogPresent(GtkWindow *parent,
    const UmiGtk4AboutDialogConfig *config)
{
    if (parent != NULL && !GTK_IS_WINDOW(parent)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = AboutConfigValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (parent != NULL && !UmiGtk4WindowIsOpen(parent)) return UMI_STATUS_CANCELLED;
    if (parent != NULL && g_object_get_data(G_OBJECT(parent), ABOUT_BUSY_KEY) != NULL)
        return UMI_STATUS_BUSY;
    GtkWindow *window = NULL;
    if (parent != NULL) {
        g_object_ref(parent);
        g_object_set_data(G_OBJECT(parent), ABOUT_BUSY_KEY, GINT_TO_POINTER(1));
        GWeakRef *cached = g_object_get_data(G_OBJECT(parent), ABOUT_CACHE_KEY);
        if (cached != NULL) window = g_weak_ref_get(cached);
        if (window != NULL && !UmiGtk4WindowIsOpen(window)) g_clear_object(&window);
        if (window != NULL && !AboutConfigMatches(window, config))
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK && window == NULL) {
        status = UmiGtk4AboutDialogCreate(parent, config, &window);
        if (status == UMI_STATUS_OK && parent != NULL) {
            GWeakRef *cached = g_new0(GWeakRef, 1);
            g_weak_ref_init(cached, window);
            g_object_set_data_full(G_OBJECT(parent), ABOUT_CACHE_KEY, cached, AboutWeakRelease);
        }
    }
    if (status == UMI_STATUS_OK) {
        if (parent != NULL) status = UmiGtk4PresentOwnedWindow(parent, window);
        else {
            gtk_window_present(window);
            if (!UmiGtk4WindowIsOpen(window) || !gtk_widget_get_visible(GTK_WIDGET(window)))
                status = UMI_STATUS_CANCELLED;
        }
    }
    g_clear_object(&window);
    if (parent != NULL) {
        g_object_set_data(G_OBJECT(parent), ABOUT_BUSY_KEY, NULL);
        g_object_unref(parent);
    }
    return status;
}

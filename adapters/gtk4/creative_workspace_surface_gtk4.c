/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_workspace_surface_gtk4.c
 *
 * PURPOSE:
 *   Expose the Media editing workspace while retaining the original layouts and shared editor ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "creative_workspace_private.h"
#include <string.h>
struct UmiCreativeGtkSurface {
    UmiCreativeGtkPanel *panel;
    GtkWidget *root, *original;
    GtkStack *pages;
};

/* The former generic composition is retained for review. The replacement
 * promotes the same Media editor into its own workspace without creating a
 * second recipe, process owner or playback session. Other profiles retain
 * their existing creative workbench. */
#if 0
UmiStatus UmiCreativeGtkSurfaceCreate(GtkWidget * original, const char * profile, UmiCreativeGtkSurface * * out)
{
    if (out == NULL)return UMI_STATUS_INVALID_ARGUMENT;
    * out = NULL;
    if (original == NULL || gtk_widget_get_parent(original)!= NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeGtkSurface * surface = g_new0(UmiCreativeGtkSurface, 1);
    UmiStatus status = UmiCreativeGtkPanelCreate(NULL, profile, & surface -> panel);
    if (status!= UMI_STATUS_OK){g_free(surface);
        return status;
    }
    surface -> root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    g_object_ref_sink(surface -> root);
    surface -> pages = GTK_STACK(gtk_stack_new());
    gtk_widget_set_vexpand(GTK_WIDGET(surface -> pages), TRUE);
    GtkWidget * switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), surface -> pages);
    gtk_widget_set_halign(switcher, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(surface -> root), switcher);
    gtk_box_append(GTK_BOX(surface -> root), GTK_WIDGET(surface -> pages));
    gtk_stack_add_titled(surface -> pages, UmiCreativeGtkPanelWidget(surface -> panel), "creative-workspace", "Creative workbench");
    gtk_stack_add_titled(surface -> pages, original, "layouts", "Existing layouts");
    surface -> original = original;
    * out = surface;
    return UMI_STATUS_OK;
}

#endif

static void ShowVideoWorkspace(GtkButton *button, gpointer pages)
{
    (void)button;
    gtk_stack_set_visible_child_name(GTK_STACK(pages), "video-editing");
}

UmiStatus UmiCreativeGtkSurfaceCreate(GtkWidget *original, const char *profile,
                                     UmiCreativeGtkSurface **out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (original == NULL || gtk_widget_get_parent(original) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiCreativeGtkSurface *surface = g_new0(UmiCreativeGtkSurface, 1);
    UmiStatus status = UmiCreativeGtkPanelCreate(NULL, profile, &surface->panel);
    if (status != UMI_STATUS_OK) { g_free(surface); return status; }
    surface->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_ref_sink(surface->root);
    surface->pages = GTK_STACK(gtk_stack_new());
    g_object_set_data(G_OBJECT(surface->pages), "umicom-automation-id", "creative.surface.pages");
    gtk_widget_set_hexpand(GTK_WIDGET(surface->pages), TRUE);
    gtk_widget_set_vexpand(GTK_WIDGET(surface->pages), TRUE);
    /* A hidden specialist page must not set the minimum size of the active
     * editor. Its widgets and model remain alive when the page is switched. */
    gtk_stack_set_hhomogeneous(surface->pages, FALSE);
    gtk_stack_set_vhomogeneous(surface->pages, FALSE);
    GtkWidget *switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(switcher), surface->pages);
    gtk_widget_set_halign(switcher, GTK_ALIGN_START);
    gtk_box_append(GTK_BOX(surface->root), switcher);
    gtk_box_append(GTK_BOX(surface->root), GTK_WIDGET(surface->pages));
    bool media = profile != NULL && strcmp(profile, "media") == 0;
    bool music = profile != NULL && strcmp(profile, "music") == 0;
    GtkWidget *video = surface->panel->videoSequence;
    GtkWidget *previous = video != NULL ? gtk_widget_get_parent(video) : NULL;
    if ((media || music) && GTK_IS_BOX(previous)) {
        /* The panel retains a strong reference across reparenting and still
         * retires all media jobs before its surface releases the widgets. */
        gtk_box_remove(GTK_BOX(previous), video);
        gtk_stack_add_titled(surface->pages, video, "video-editing", "Video editing");
        GtkWidget *open = gtk_button_new_with_label("Open video editing workspace");
        g_signal_connect_object(open, "clicked", G_CALLBACK(ShowVideoWorkspace), surface->pages, 0);
        gtk_box_append(GTK_BOX(previous), open);
    }
    gtk_stack_add_titled(surface->pages, UmiCreativeGtkPanelWidget(surface->panel),
                        "creative-workspace", media ? "Design and generators" : "Creative workbench");
    gtk_stack_add_titled(surface->pages, original, "layouts", media ? "Workspace layouts" : "Existing layouts");
    /* Music keeps its audio/design landing view while video editing receives
     * the same full-width workspace instead of enlarging its sidebar. */
    if (music) gtk_stack_set_visible_child_name(surface->pages, "creative-workspace");
    surface->original = original;
    *out = surface;
    return UMI_STATUS_OK;
}

GtkWidget * UmiCreativeGtkSurfaceWidget(UmiCreativeGtkSurface * surface){return surface!= NULL ? surface -> root : NULL;
}

void UmiCreativeGtkSurfaceDestroy(UmiCreativeGtkSurface * surface)
{
    if (surface == NULL)return;
    if (surface -> original!= NULL && gtk_widget_get_parent(surface -> original) == GTK_WIDGET(surface -> pages))gtk_stack_remove(surface -> pages, surface -> original);
    surface -> original = NULL;
    UmiCreativeGtkPanelDestroy(surface -> panel);
    g_object_unref(surface -> root);
    g_free(surface);
}


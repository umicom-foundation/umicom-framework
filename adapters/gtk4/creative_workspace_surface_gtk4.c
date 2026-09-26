/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_workspace_surface_gtk4.c
 * PURPOSE: Preserve existing product layouts while composing the shared creative workbench.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/creative_workspace.h"
struct UmiCreativeGtkSurface {
    UmiCreativeGtkPanel * panel;
    GtkWidget * root, * original;
    GtkStack * pages;
};

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


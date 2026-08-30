/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/asset_browser_gtk4.c
 *
 * PURPOSE:
 *   Render a reusable asset-browser gallery for images, video, audio, models, materials, presets and project resources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <inttypes.h>
#include <stdio.h>
#include "umicom/ui/gtk4/workstation/asset_browser.h"

GtkWidget *umi_gtk4_ws_asset_browser_create(const UmiWsAssetBrowserModel *model) {
    GtkWidget *scroller = gtk_scrolled_window_new();
    GtkWidget *flow = gtk_flow_box_new();
    size_t i;
    gtk_widget_add_css_class(flow, "umicom-asset-browser");
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flow), GTK_SELECTION_MULTIPLE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(flow), 1U);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(flow), 8U);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), flow);
    if (model == NULL) return scroller;
    for (i = 0U; i < model->count; ++i) {
        char size_text[64];
        GtkWidget *tile = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
        GtkWidget *thumb = gtk_image_new_from_icon_name(model->assets[i].kind == UMI_WS_ASSET_AUDIO ? "audio-x-generic-symbolic" : "image-x-generic-symbolic");
        GtkWidget *label = gtk_label_new(model->assets[i].label);
        GtkWidget *meta;
        (void)snprintf(size_text, sizeof(size_text), "%" PRIu64 " bytes", model->assets[i].size_bytes);
        meta = gtk_label_new(size_text);
        gtk_widget_add_css_class(tile, "umicom-asset-tile");
        gtk_widget_add_css_class(meta, "dim-label");
        gtk_box_append(GTK_BOX(tile), thumb);
        gtk_box_append(GTK_BOX(tile), label);
        gtk_box_append(GTK_BOX(tile), meta);
        gtk_flow_box_append(GTK_FLOW_BOX(flow), tile);
    }
    return scroller;
}

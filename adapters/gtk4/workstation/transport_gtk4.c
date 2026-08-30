/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/transport_gtk4.c
 *
 * PURPOSE:
 *   Render media/replay transport controls from the generic transport model without embedding domain logic in GTK4.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "umicom/ui/gtk4/workstation/transport.h"

GtkWidget *umi_gtk4_ws_transport_create(const UmiWsTransportModel *model) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    GtkWidget *back = gtk_button_new_from_icon_name("media-skip-backward-symbolic");
    GtkWidget *play = gtk_button_new_from_icon_name(model != NULL && model->state == UMI_WS_TRANSPORT_PLAYING ? "media-playback-pause-symbolic" : "media-playback-start-symbolic");
    GtkWidget *stop = gtk_button_new_from_icon_name("media-playback-stop-symbolic");
    GtkWidget *forward = gtk_button_new_from_icon_name("media-skip-forward-symbolic");
    GtkWidget *scale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, model != NULL && model->duration_ms > 0 ? (double)model->duration_ms : 1.0, 1.0);
    gtk_widget_add_css_class(box, "umicom-transport");
    gtk_widget_set_hexpand(scale, TRUE);
    if (model != NULL) gtk_range_set_value(GTK_RANGE(scale), (double)model->position_ms);
    gtk_box_append(GTK_BOX(box), back); gtk_box_append(GTK_BOX(box), play); gtk_box_append(GTK_BOX(box), stop);
    gtk_box_append(GTK_BOX(box), forward); gtk_box_append(GTK_BOX(box), scale);
    return box;
}

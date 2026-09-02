/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/timeline_gtk4.c
 *
 * PURPOSE:
 *   Render reusable multi-track timeline state for video editing, music production, animation and replay workstations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "umicom/ui/gtk4/workstation/timeline.h"

/*
 * Initialise gtk4 ws timeline from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_timeline_create(const UmiWsTimelineModel *timeline) {
    GtkWidget *scroller = gtk_scrolled_window_new();
    GtkWidget *tracks = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    size_t i;
    gtk_widget_add_css_class(tracks, "umicom-timeline");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), tracks);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (timeline == NULL) return scroller;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < timeline->track_count; ++i) {
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
        GtkWidget *label = gtk_label_new(timeline->tracks[i].label);
        GtkWidget *lane = gtk_level_bar_new_for_interval(0.0, 1.0);
        gtk_widget_add_css_class(row, "umicom-timeline-track");
        gtk_widget_set_size_request(label, 120, -1);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        gtk_level_bar_set_value(GTK_LEVEL_BAR(lane), timeline->tracks[i].muted ? 0.2 : 0.75);
        gtk_widget_set_hexpand(lane, TRUE);
        gtk_box_append(GTK_BOX(row), label);
        gtk_box_append(GTK_BOX(row), lane);
        gtk_box_append(GTK_BOX(tracks), row);
    }
    return scroller;
}

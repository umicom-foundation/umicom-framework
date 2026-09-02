/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/data_grid_gtk4.c
 *
 * PURPOSE:
 *   Render semantic data-grid column definitions for trading tables, databases, assets and operations lists.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include "umicom/ui/gtk4/workstation/data_grid.h"

/*
 * Initialise gtk4 ws data grid from caller-provided values so later operations receive a
 * known state.
 */
GtkWidget *umi_gtk4_ws_data_grid_create(const UmiWsDataGridSurface *grid) {
    GtkWidget *scroller = gtk_scrolled_window_new();
    GtkWidget *table = gtk_grid_new();
    size_t c;
    char rows[64];
    gtk_widget_add_css_class(table, "umicom-data-grid");
    gtk_grid_set_column_spacing(GTK_GRID(table), 1);
    gtk_grid_set_row_spacing(GTK_GRID(table), 1);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroller), table);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (grid == NULL) return scroller;
    /* Visit each bounded item once so every record receives the same rule. */
    for (c = 0U; c < grid->column_count; ++c) {
        GtkWidget *header = gtk_label_new(grid->columns[c].label);
        gtk_widget_add_css_class(header, "umicom-data-grid-header");
        gtk_widget_set_size_request(header, grid->columns[c].width, 28);
        gtk_widget_set_hexpand(header, TRUE);
        gtk_grid_attach(GTK_GRID(table), header, (int)c, 0, 1, 1);
    }
    (void)snprintf(rows, sizeof(rows), "%zu rows", grid->row_count);
    /* Apply this branch only when its contract condition is satisfied. */
    if (grid->column_count > 0U) {
        GtkWidget *summary = gtk_label_new(rows);
        gtk_widget_add_css_class(summary, "dim-label");
        gtk_grid_attach(GTK_GRID(table), summary, 0, 1, (int)grid->column_count, 1);
    }
    return scroller;
}

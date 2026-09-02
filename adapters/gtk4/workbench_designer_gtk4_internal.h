/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workbench_designer_gtk4_internal.h
 *
 * PURPOSE:
 *   Share private GTK4 widget composition, refresh and command-dispatch helpers
 *   across Layout Designer adapter implementation units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_GTK4_INTERNAL_H
#define UMICOM_WORKBENCH_DESIGNER_GTK4_INTERNAL_H

#include <gtk/gtk.h>

#include "umicom/ui/workbench_designer_gtk4.h"

struct UmiWorkbenchLayoutBrowserGtk4 {
    GtkWidget *root;
    GtkWidget *search_entry;
    GtkWidget *category_combo;
    GtkWidget *list_box;
    GtkWidget *preview_frame;
    GtkWidget *preview_area;
    GtkWidget *name_label;
    GtkWidget *description_label;
    GtkWidget *open_button;
    GtkWidget *clone_button;
    GtkWidget *delete_button;
    UmiWorkbenchDesignerController *controller;
    UmiWorkbenchDesignerBrowser model;
    uint64_t revision;
};

struct UmiWorkbenchDesignerGtk4 {
    GtkWidget *root;
    GtkWidget *browser_revealer;
    GtkWidget *browser_widget;
    GtkWidget *toolbar;
    GtkWidget *work_area;
    GtkWidget *palette_box;
    GtkWidget *palette_search;
    GtkWidget *palette_list;
    GtkWidget *canvas_overlay;
    GtkWidget *canvas_area;
    GtkWidget *docking_area;
    GtkWidget *minimap_area;
    GtkWidget *tree_box;
    GtkWidget *tree_list;
    GtkWidget *properties_box;
    GtkWidget *property_grid;
    GtkWidget *history_list;
    GtkWidget *diagnostics_list;
    GtkWidget *collaboration_box;
    GtkWidget *status_label;
    GtkWidget *zoom_label;
    GtkWidget *command_popover;
    GtkWidget *command_search;
    GtkWidget *command_list;
    UmiWorkbenchDesignerGtk4Config config;
    UmiWorkbenchDesignerModelBundle bundle;
    UmiWorkbenchLayoutBrowserGtk4 *browser;
    uint64_t revision;
};

/**
 * Provide the workbench designer gtk4 build toolbar operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_toolbar(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 build palette operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_palette(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 build canvas operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_canvas(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 build tree operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_tree(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 build properties operation used by this module and
 * its client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_properties(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 build review operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_review(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 build status operation used by this module and its
 * client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_status(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 build command palette operation used by this module
 * and its client applications.
 */
GtkWidget *umi_workbench_designer_gtk4_build_command_palette(
    UmiWorkbenchDesignerGtk4 *designer);

/**
 * Provide the workbench designer gtk4 refresh toolbar operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_gtk4_refresh_toolbar(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 refresh palette operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_gtk4_refresh_palette(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 refresh tree operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_refresh_tree(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 refresh properties operation used by this module and
 * its client applications.
 */
void umi_workbench_designer_gtk4_refresh_properties(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 refresh review operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_refresh_review(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 refresh status operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_refresh_status(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Provide the workbench designer gtk4 refresh command palette operation used by this
 * module and its client applications.
 */
void umi_workbench_designer_gtk4_refresh_command_palette(
    UmiWorkbenchDesignerGtk4 *designer);
/**
 * Perform workbench designer gtk4 through the module contract so client applications do
 * not duplicate its policy.
 */
void umi_workbench_designer_gtk4_dispatch(
    UmiWorkbenchDesignerGtk4 *designer,
    const char *command_id);
/**
 * Provide the workbench designer gtk4 clear box operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_clear_box(GtkWidget *box);

/**
 * Provide the workbench designer gtk4 canvas draw operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_canvas_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data);
/**
 * Provide the workbench designer gtk4 minimap draw operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_minimap_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data);
/**
 * Provide the workbench designer gtk4 docking draw operation used by this module and its
 * client applications.
 */
void umi_workbench_designer_gtk4_docking_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data);
/**
 * Provide the workbench layout browser gtk4 preview draw operation used by this module and
 * its client applications.
 */
void umi_workbench_layout_browser_gtk4_preview_draw(
    GtkDrawingArea *area,
    cairo_t *cr,
    int width,
    int height,
    gpointer user_data);

#endif

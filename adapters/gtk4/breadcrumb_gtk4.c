/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/breadcrumb_gtk4.c
 *
 * PURPOSE:
 *   Render the Framework breadcrumb model above the central document area.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Breadcrumb segments are display-only in this first reusable adapter pass. Products can later bind segment activation to navigation commands.
 */

#include "gtk4_internal.h"

/*
 * Provide the gtk4 refresh breadcrumbs operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_breadcrumbs(UmiGtk4Adapter *adapter,
                                      UmiUiWorkbench *workbench)
{
    UmiUiBreadcrumbSnapshot snapshot;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_gtk4_clear_box(adapter->breadcrumb_box);
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ui_breadcrumb_snapshot(umi_ui_workbench_breadcrumbs(workbench),
                                   &snapshot) != UMI_STATUS_OK) {
        return UMI_STATUS_OK;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < snapshot.count; ++index) {
        GtkWidget *label;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index > 0U) {
            GtkWidget *separator = gtk_label_new("›");
            gtk_widget_add_css_class(separator, "dim-label");
            gtk_widget_add_css_class(separator, "umicom-breadcrumb-separator");
            gtk_box_append(GTK_BOX(adapter->breadcrumb_box), separator);
        }
        label = gtk_label_new(snapshot.segments[index].label);
        gtk_widget_add_css_class(label, "umicom-breadcrumb-segment");
        gtk_widget_set_tooltip_text(label, snapshot.segments[index].path);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (snapshot.segments[index].current) {
            gtk_widget_add_css_class(label, "heading");
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            gtk_widget_add_css_class(label, "dim-label");
        }
        gtk_box_append(GTK_BOX(adapter->breadcrumb_box), label);
    }
    return UMI_STATUS_OK;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/pane_gtk4.c
 *
 * PURPOSE:
 *   Map toolkit-neutral pane metadata and registered view-model presentations
 *   into left, right and bottom GTK4 workbench containers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Centre documents are rendered by the document/editor adapter. Side and bottom
 * tools share tabbed notebook hosts so several visible tools do not compete for
 * height. Pane identity, order, visibility and content remain owned by Framework
 * models, while splitter dimensions are handled by persisted workbench state.
 */

#include "gtk4_internal.h"

static GtkWidget *pane_target(UmiGtk4Adapter *adapter, UmiUiPlacement placement)
{
    if (placement == UMI_UI_PLACEMENT_LEFT) return adapter->left_box;
    if (placement == UMI_UI_PLACEMENT_RIGHT) return adapter->right_box;
    if (placement == UMI_UI_PLACEMENT_BOTTOM ||
        placement == UMI_UI_PLACEMENT_TOP) {
        return adapter->bottom_box;
    }

    /*
     * UMI_UI_PLACEMENT_CENTRE belongs to the document/editor host. Previously
     * it fell through to the left sidebar, which could render the Editor as a
     * second sidebar pane.
     */
    return NULL;
}

static void clear_notebook(GtkWidget *notebook)
{
    int pages;
    if (notebook == NULL || !GTK_IS_NOTEBOOK(notebook)) return;
    pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    while (pages > 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), 0);
        pages -= 1;
    }
}

static GtkWidget *create_tab_label(const UmiUiPaneSnapshot *pane)
{
    GtkWidget *tab_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label = gtk_label_new(pane->title);

    gtk_widget_add_css_class(tab_box, "umicom-tool-tab");
    if (pane->icon_name[0] != '\0') {
        GtkWidget *icon = gtk_image_new_from_icon_name(pane->icon_name);
        gtk_image_set_pixel_size(GTK_IMAGE(icon), 14);
        gtk_box_append(GTK_BOX(tab_box), icon);
    }
    gtk_box_append(GTK_BOX(tab_box), label);
    gtk_widget_set_tooltip_text(tab_box, pane->title);
    return tab_box;
}

UmiStatus umi_gtk4_refresh_panes(UmiGtk4Adapter *adapter,
                                 UmiUiWorkbench *workbench)
{
    UmiUiPaneModel *model;
    size_t index;

    if (adapter == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    clear_notebook(adapter->left_box);
    clear_notebook(adapter->right_box);
    clear_notebook(adapter->bottom_box);

    model = umi_ui_workbench_panes(workbench);
    for (index = 0U; index < umi_ui_pane_model_count(model); ++index) {
        UmiUiPaneSnapshot pane;
        UmiStatus status;

        status = umi_ui_pane_model_at(model, index, &pane);
        if (status != UMI_STATUS_OK) return status;
        if (!pane.visible) continue;

        {
            GtkWidget *target = pane_target(adapter, pane.placement);
            GtkWidget *page;
            GtkWidget *tab;
            GtkWidget *content = NULL;
            int page_index;

            if (target == NULL) continue;

            status = umi_gtk4_build_view_widget(adapter, workbench,
                                                &pane, &content);
            if (status != UMI_STATUS_OK) return status;

            page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
            gtk_widget_add_css_class(page, "umicom-tool-page");
            gtk_widget_set_hexpand(page, TRUE);
            gtk_widget_set_vexpand(page, TRUE);
            gtk_widget_set_hexpand(content, TRUE);
            gtk_widget_set_vexpand(content, TRUE);
            gtk_box_append(GTK_BOX(page), content);

            tab = create_tab_label(&pane);
            page_index = gtk_notebook_append_page(GTK_NOTEBOOK(target),
                                                  page,
                                                  tab);
            g_object_set_data_full(G_OBJECT(page),
                                   "umicom-pane-id",
                                   g_strdup(pane.pane_id),
                                   g_free);
            if (page_index == 0) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(target), 0);
            }
        }
    }
    return UMI_STATUS_OK;
}

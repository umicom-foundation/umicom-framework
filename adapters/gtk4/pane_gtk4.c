/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/pane_gtk4.c
 *
 * PURPOSE:
 *   Map toolkit-neutral pane metadata and registered view-model presentations
 *   into left, right and bottom GTK4 workbench containers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Centre documents are rendered by the document/editor adapter. Side and bottom
 * tools share tabbed notebook hosts so several visible tools do not compete for
 * height. Pane identity, order, visibility and content remain owned by Framework
 * models, while splitter dimensions are handled by persisted workbench state.
 */

#include "gtk4_internal.h"

#include <stdlib.h>

static void on_dock_page_added(GtkNotebook *notebook,
                               GtkWidget *page,
                               guint page_number,
                               gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *pane_id;
    UmiUiPlacement placement;
    UmiUiWorkbench *workbench;

    if (adapter == NULL || adapter->shell == NULL ||
        adapter->applying_dock_state) {
        return;
    }
    pane_id = (const char *)g_object_get_data(
        G_OBJECT(page), "umicom-pane-id");
    placement = (UmiUiPlacement)GPOINTER_TO_INT(g_object_get_data(
        G_OBJECT(notebook), "umicom-dock-placement"));
    if (pane_id == NULL) return;

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    if (umi_ui_workbench_dock_pane(workbench,
                                   pane_id,
                                   placement,
                                   (int32_t)page_number * 10) == UMI_STATUS_OK) {
        /* The GTK drag already moved the actual page. Refresh only the layout
         * label so the unsaved-change marker appears without rebuilding the
         * notebooks during an active drag operation. */
        (void)umi_gtk4_refresh_workspace_profiles(adapter, workbench);
    }
}

static void on_dock_page_reordered(GtkNotebook *notebook,
                                   GtkWidget *page,
                                   guint page_number,
                                   gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    UmiUiWorkbench *workbench;
    UmiUiPlacement placement;
    int index;
    int page_count;

    (void)page;
    (void)page_number;
    if (adapter == NULL || adapter->shell == NULL ||
        adapter->applying_dock_state) {
        return;
    }
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    placement = (UmiUiPlacement)GPOINTER_TO_INT(g_object_get_data(
        G_OBJECT(notebook), "umicom-dock-placement"));
    page_count = gtk_notebook_get_n_pages(notebook);
    for (index = 0; index < page_count; ++index) {
        GtkWidget *child = gtk_notebook_get_nth_page(notebook, index);
        const char *pane_id = child == NULL ? NULL :
            (const char *)g_object_get_data(
                G_OBJECT(child), "umicom-pane-id");
        if (pane_id != NULL) {
            (void)umi_ui_workbench_dock_pane(
                workbench, pane_id, placement, (int32_t)index * 10);
        }
    }
    (void)umi_gtk4_refresh_workspace_profiles(adapter, workbench);
}

void umi_gtk4_configure_dock_notebook(UmiGtk4Adapter *adapter,
                                      GtkWidget *notebook,
                                      UmiUiPlacement placement)
{
    if (adapter == NULL || notebook == NULL || !GTK_IS_NOTEBOOK(notebook)) {
        return;
    }
    gtk_notebook_set_group_name(GTK_NOTEBOOK(notebook),
                                "umicom-workbench-tool-docks");
    g_object_set_data(G_OBJECT(notebook),
                      "umicom-dock-placement",
                      GINT_TO_POINTER((int)placement));
    g_signal_connect(notebook,
                     "page-added",
                     G_CALLBACK(on_dock_page_added),
                     adapter);
    g_signal_connect(notebook,
                     "page-reordered",
                     G_CALLBACK(on_dock_page_reordered),
                     adapter);
}

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

static int compare_panes(const void *left, const void *right)
{
    const UmiUiPaneSnapshot *first = (const UmiUiPaneSnapshot *)left;
    const UmiUiPaneSnapshot *second = (const UmiUiPaneSnapshot *)right;
    if (first->placement != second->placement) {
        return (int)first->placement - (int)second->placement;
    }
    if (first->order != second->order) {
        return first->order < second->order ? -1 : 1;
    }
    return g_strcmp0(first->title, second->title);
}

UmiStatus umi_gtk4_refresh_panes(UmiGtk4Adapter *adapter,
                                 UmiUiWorkbench *workbench)
{
    UmiUiPaneModel *model;
    UmiUiPaneSnapshot *panes;
    size_t count;
    size_t index;

    if (adapter == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    model = umi_ui_workbench_panes(workbench);
    count = umi_ui_pane_model_count(model);
    panes = count == 0U ? NULL :
        (UmiUiPaneSnapshot *)calloc(count, sizeof(*panes));
    if (count > 0U && panes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    for (index = 0U; index < count; ++index) {
        UmiStatus status = umi_ui_pane_model_at(model, index, &panes[index]);
        if (status != UMI_STATUS_OK) {
            free(panes);
            return status;
        }
    }
    if (count > 1U) qsort(panes, count, sizeof(*panes), compare_panes);

    adapter->applying_dock_state = 1;
    clear_notebook(adapter->left_box);
    clear_notebook(adapter->right_box);
    clear_notebook(adapter->bottom_box);

    for (index = 0U; index < count; ++index) {
        UmiUiPaneSnapshot pane = panes[index];
        UmiStatus status;
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
            if (status != UMI_STATUS_OK) {
                adapter->applying_dock_state = 0;
                free(panes);
                return status;
            }

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
            gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(target),
                                             page,
                                             pane.movable != 0);
            gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(target),
                                            page,
                                            pane.movable != 0);
            if (page_index == 0) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(target), 0);
            }
        }
    }
    adapter->applying_dock_state = 0;
    free(panes);
    return UMI_STATUS_OK;
}

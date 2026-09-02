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

/*
 * Provide the on dock page added operation used by this module and its client
 * applications.
 */
static void on_dock_page_added(GtkNotebook *notebook,
                               GtkWidget *page,
                               guint page_number,
                               gpointer user_data)
{
    UmiGtk4Adapter *adapter = (UmiGtk4Adapter *)user_data;
    const char *pane_id;
    UmiUiPlacement placement;
    UmiUiWorkbench *workbench;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL ||
        adapter->applying_dock_state) {
        return;
    }
    pane_id = (const char *)g_object_get_data(
        G_OBJECT(page), "umicom-pane-id");
    placement = (UmiUiPlacement)GPOINTER_TO_INT(g_object_get_data(
        G_OBJECT(notebook), "umicom-dock-placement"));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (pane_id == NULL) return;

    workbench = umi_ui_application_shell_workbench(adapter->shell);
    /* Apply this branch only when its contract condition is satisfied. */
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

/*
 * Provide the on dock page reordered operation used by this module and its client
 * applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || adapter->shell == NULL ||
        adapter->applying_dock_state) {
        return;
    }
    workbench = umi_ui_application_shell_workbench(adapter->shell);
    placement = (UmiUiPlacement)GPOINTER_TO_INT(g_object_get_data(
        G_OBJECT(notebook), "umicom-dock-placement"));
    page_count = gtk_notebook_get_n_pages(notebook);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0; index < page_count; ++index) {
        GtkWidget *child = gtk_notebook_get_nth_page(notebook, index);
        const char *pane_id = child == NULL ? NULL :
            (const char *)g_object_get_data(
                G_OBJECT(child), "umicom-pane-id");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (pane_id != NULL) {
            (void)umi_ui_workbench_dock_pane(
                workbench, pane_id, placement, (int32_t)index * 10);
        }
    }
    (void)umi_gtk4_refresh_workspace_profiles(adapter, workbench);
}

/*
 * Provide the gtk4 configure dock notebook operation used by this module and its client
 * applications.
 */
void umi_gtk4_configure_dock_notebook(UmiGtk4Adapter *adapter,
                                      GtkWidget *notebook,
                                      UmiUiPlacement placement)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/* Provide the pane target operation used by this module and its client applications. */
static GtkWidget *pane_target(UmiGtk4Adapter *adapter, UmiUiPlacement placement)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (placement == UMI_UI_PLACEMENT_LEFT) return adapter->left_box;
    /* Apply this branch only when its contract condition is satisfied. */
    if (placement == UMI_UI_PLACEMENT_RIGHT) return adapter->right_box;
    /* Apply this branch only when its contract condition is satisfied. */
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

/* Provide the clear notebook operation used by this module and its client applications. */
static void clear_notebook(GtkWidget *notebook)
{
    int pages;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (notebook == NULL || !GTK_IS_NOTEBOOK(notebook)) return;
    pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (pages > 0) {
        gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), 0);
        pages -= 1;
    }
}

/* Provide the create tab label operation used by this module and its client applications. */
static GtkWidget *create_tab_label(const UmiUiPaneSnapshot *pane)
{
    GtkWidget *tab_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label = gtk_label_new(pane->title);

    gtk_widget_add_css_class(tab_box, "umicom-tool-tab");
    /* Apply this branch only when its contract condition is satisfied. */
    if (pane->icon_name[0] != '\0') {
        GtkWidget *icon = gtk_image_new_from_icon_name(pane->icon_name);
        gtk_image_set_pixel_size(GTK_IMAGE(icon), 14);
        gtk_box_append(GTK_BOX(tab_box), icon);
    }
    gtk_box_append(GTK_BOX(tab_box), label);
    gtk_widget_set_tooltip_text(tab_box, pane->title);
    return tab_box;
}

/* Provide the compare panes operation used by this module and its client applications. */
static int compare_panes(const void *left, const void *right)
{
    const UmiUiPaneSnapshot *first = (const UmiUiPaneSnapshot *)left;
    const UmiUiPaneSnapshot *second = (const UmiUiPaneSnapshot *)right;
    /* Apply this branch only when its contract condition is satisfied. */
    if (first->placement != second->placement) {
        return (int)first->placement - (int)second->placement;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (first->order != second->order) {
        return first->order < second->order ? -1 : 1;
    }
    return g_strcmp0(first->title, second->title);
}

/*
 * Provide the gtk4 refresh panes operation used by this module and its client
 * applications.
 */
UmiStatus umi_gtk4_refresh_panes(UmiGtk4Adapter *adapter,
                                 UmiUiWorkbench *workbench)
{
    UmiUiPaneModel *model;
    UmiUiPaneSnapshot *panes;
    size_t count;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || workbench == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    model = umi_ui_workbench_panes(workbench);
    count = umi_ui_pane_model_count(model);
    panes = count == 0U ? NULL :
        (UmiUiPaneSnapshot *)calloc(count, sizeof(*panes));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (count > 0U && panes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiStatus status = umi_ui_pane_model_at(model, index, &panes[index]);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            free(panes);
            return status;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > 1U) qsort(panes, count, sizeof(*panes), compare_panes);

    adapter->applying_dock_state = 1;
    clear_notebook(adapter->left_box);
    clear_notebook(adapter->right_box);
    clear_notebook(adapter->bottom_box);

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        UmiUiPaneSnapshot pane = panes[index];
        UmiStatus status;
        /* Apply this operation only while the related capability or state is available. */
        if (!pane.visible) continue;

        {
            GtkWidget *target = pane_target(adapter, pane.placement);
            GtkWidget *page;
            GtkWidget *tab;
            GtkWidget *content = NULL;
            int page_index;

            /* Configure the optional target only when its feature has created it. */
            if (target == NULL) continue;

            status = umi_gtk4_build_view_widget(adapter, workbench,
                                                &pane, &content);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
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
            /* Apply this branch only when its contract condition is satisfied. */
            if (page_index == 0) {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(target), 0);
            }
        }
    }
    adapter->applying_dock_state = 0;
    free(panes);
    return UMI_STATUS_OK;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/tab_host_gtk4.c
 *
 * PURPOSE:
 *   Render accessible GTK4 tab groups for panels and documents, including
 *   model-routed close buttons for editable workspace tabs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/tab_host.h"

#include <stdbool.h>
#include <stdio.h>

#include "umicom/ui/gtk4/automation.h"

typedef struct UmiGtk4WorkstationTabCloseData {
    /* A stale tab must not close a panel in a newly rebuilt layout. */
    guint pending_id;
    UmiGtk4WorkstationTabCloseHandler handler;
    void *user_data;
    char tab_id[UMI_UI_ID_CAPACITY];
} UmiGtk4WorkstationTabCloseData;

typedef struct UmiGtk4WorkstationPendingTabClose {
    UmiGtk4WorkstationTabCloseData *owner;
    UmiGtk4WorkstationTabCloseHandler handler;
    void *user_data;
    char tab_id[UMI_UI_ID_CAPACITY];
} UmiGtk4WorkstationPendingTabClose;


/* Widget data owns the signal's borrowed close record and pending request. */
static void close_data_destroy(gpointer data)
{
    UmiGtk4WorkstationTabCloseData *close_data = data;
    if (close_data != NULL && close_data->pending_id != 0U)
        g_source_remove(close_data->pending_id);
    g_free(data);
}

/* Old retained tab buttons must not close a new layout's matching instance. */
void umi_gtk4_ws_tab_host_invalidate_actions(GtkWidget *root)
{
    GtkWidget *child;
    UmiGtk4WorkstationTabCloseData *data;
    if (root == NULL) return;
    data = g_object_get_data(G_OBJECT(root), "umicom-tab-close-data");
    if (data != NULL) {
        if (data->pending_id != 0U) g_source_remove(data->pending_id);
        data->pending_id = 0U;
        data->handler = NULL;
        data->user_data = NULL;
    }
    for (child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
        umi_gtk4_ws_tab_host_invalidate_actions(child);
}

/* Dispatch after the button signal returns so rebuilding the owning layout
 * cannot destroy the button while GTK is still emitting its click signal. */
static gboolean dispatch_close_from_idle(gpointer user_data)
{
    UmiGtk4WorkstationPendingTabClose *pending =
        (UmiGtk4WorkstationPendingTabClose *)user_data;

    /* The owner may disappear during its callback. Do not access it after
     * dispatch, and do not try to cancel the idle that is already executing. */
    if (pending != NULL && pending->owner != NULL)
        pending->owner->pending_id = 0U;

    if (pending != NULL && pending->handler != NULL) {
        pending->handler(pending->tab_id, pending->user_data);
    }
    return G_SOURCE_REMOVE;
}

/* Route a close request through the layout owner rather than directly
 * removing the GTK page and leaving the portable model out of sync. */
static void on_close_clicked(GtkButton *button, gpointer user_data)
{
    UmiGtk4WorkstationTabCloseData *data =
        (UmiGtk4WorkstationTabCloseData *)user_data;

    UmiGtk4WorkstationPendingTabClose *pending;

    (void)button;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (data == NULL || data->handler == NULL || data->pending_id != 0U) return;
    pending = g_new0(UmiGtk4WorkstationPendingTabClose, 1);
    if (pending == NULL) return;
    pending->handler = data->handler;
    pending->owner = data;
    pending->user_data = data->user_data;
    (void)snprintf(
        pending->tab_id, sizeof(pending->tab_id), "%s", data->tab_id);
    data->pending_id = g_idle_add_full(
            G_PRIORITY_DEFAULT_IDLE,
            dispatch_close_from_idle,
            pending,
            g_free);
    if (data->pending_id == 0U) {
        g_free(pending);
    }
}

/* Build one readable tab label, with the close action placed beside its title
 * only when the owning layout permits removal. */
static GtkWidget *create_tab_label(
    const char *tab_id,
    const char *title,
    bool closable,
    UmiGtk4WorkstationTabCloseHandler close_handler,
    void *user_data)
{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *close_button;
    UmiGtk4WorkstationTabCloseData *close_data;
    char automation_id[UMI_UI_ID_CAPACITY + 32U];

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    label = gtk_label_new(title);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (box == NULL || label == NULL) return box;
    gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
    gtk_widget_set_hexpand(label, TRUE);
    gtk_box_append(GTK_BOX(box), label);
    /* Apply this branch only when its contract condition is satisfied. */
    if (!closable || close_handler == NULL) return box;

    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (close_button == NULL) return box;
    gtk_widget_add_css_class(close_button, "flat");
    gtk_widget_add_css_class(close_button, "umicom-tab-close");
    gtk_widget_set_tooltip_text(close_button, "Close panel");
    close_data = g_new0(UmiGtk4WorkstationTabCloseData, 1);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (close_data == NULL) return box;
    close_data->handler = close_handler;
    close_data->user_data = user_data;
    (void)snprintf(
        close_data->tab_id, sizeof(close_data->tab_id), "%s", tab_id);
    g_object_set_data_full(G_OBJECT(close_button), "umicom-tab-close-data",
                           close_data, close_data_destroy);
    g_signal_connect(close_button, "clicked", G_CALLBACK(on_close_clicked), close_data);
    (void)snprintf(
        automation_id, sizeof(automation_id), "%s.tab.close", tab_id);
    (void)umi_gtk4_automation_tag_widget(close_button, automation_id);
    gtk_box_append(GTK_BOX(box), close_button);
    return box;
}

GtkWidget *umi_gtk4_ws_tab_host_create(const UmiWsTabStack *stack)
{
    GtkWidget *notebook = gtk_notebook_new();
    gtk_widget_add_css_class(notebook, "umicom-tab-host");
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
    gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (notebook != NULL) {
        gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
        gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
        gtk_widget_add_css_class(notebook, "umicom-workstation-tab-host");
        if (stack != NULL && stack->active_index < stack->count) {
            gtk_notebook_set_current_page(
                GTK_NOTEBOOK(notebook), (int)stack->active_index);
        }
    }
    return notebook;
}

UmiStatus umi_gtk4_ws_tab_host_append_managed(
    GtkWidget *notebook,
    const char *tab_id,
    const char *title,
    GtkWidget *child,
    bool closable,
    UmiGtk4WorkstationTabCloseHandler close_handler,
    void *user_data)
{
    GtkWidget *tab_label;
    int page_index;
    char automation_id[UMI_UI_ID_CAPACITY + 32U];

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (notebook == NULL || !GTK_IS_NOTEBOOK(notebook) ||
        tab_id == NULL || tab_id[0] == '\0' ||
        title == NULL || child == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    tab_label = create_tab_label(
        tab_id, title, closable, close_handler, user_data);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tab_label == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    page_index = gtk_notebook_append_page(
        GTK_NOTEBOOK(notebook), child, tab_label);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (page_index < 0) return UMI_STATUS_INVALID_STATE;
    (void)snprintf(
        automation_id, sizeof(automation_id), "%s.tab", tab_id);
    (void)umi_gtk4_automation_tag_widget(child, automation_id);
    return UMI_STATUS_OK;
}

UmiStatus umi_gtk4_ws_tab_host_append(
    GtkWidget *notebook,
    const char *title,
    GtkWidget *child)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (notebook == NULL || title == NULL || child == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_gtk4_ws_tab_host_append_managed(
        notebook,
        title,
        title,
        child,
        false,
        NULL,
        NULL);
}

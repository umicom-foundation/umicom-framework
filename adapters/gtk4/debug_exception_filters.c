/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_exception_filters.c
 * PURPOSE: Render and apply adapter exception choices through an explicitly owned host.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/debug_exception_filters.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>
typedef struct ExceptionPanel
{
    GtkWidget *root;
    GtkWidget *rows;
    GtkWidget *status;
    GtkWidget *apply_button;
    GtkWidget *checks[UMI_DEBUG_EXCEPTION_FILTER_LIMIT];
    UmiDebugExceptionSnapshot snapshot;
    UmiGtk4ExceptionFiltersRead read;
    UmiGtk4ExceptionFiltersApply apply;
    void *context;
    GDestroyNotify destroy;
    gboolean captured;
    gboolean pending;
} ExceptionPanel;
static void ExceptionPanelFree(gpointer data)
{
    ExceptionPanel *panel = data;
    if (panel->destroy != NULL)
        panel->destroy(panel->context);
    g_free(panel);
}
static void ExceptionPanelClear(ExceptionPanel *panel)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->rows)) != NULL)
        gtk_box_remove(GTK_BOX(panel->rows), child);
    memset(panel->checks, 0, sizeof panel->checks);
    panel->captured = FALSE;
    gtk_widget_set_sensitive(panel->apply_button, FALSE);
}
static void ExceptionPanelRender(ExceptionPanel *panel, const UmiDebugExceptionSnapshot *snapshot)
{
    ExceptionPanelClear(panel);
    panel->snapshot = *snapshot;
    panel->captured = TRUE;
    size_t selected = 0U;
    for (size_t i = 0U; i < snapshot->catalog.count; ++i)
    {
        const UmiDebugExceptionFilter *filter = &snapshot->catalog.items[i];
        GtkWidget *check = gtk_check_button_new_with_label(filter->label);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(check), snapshot->selection.enabled[i] != 0);
        gtk_widget_set_tooltip_text(check, filter->description);
        panel->checks[i] = check;
        gtk_box_append(GTK_BOX(panel->rows), check);
        char tag[80];
        g_snprintf(tag, sizeof tag, "debug.exceptions.filter.%zu", i);
        (void)umi_gtk4_automation_tag_widget(check, tag);
        if (snapshot->selection.enabled[i])
        {
            int verified = -1;
            const char *message = "";
            if (snapshot->acknowledged && selected < snapshot->acknowledgement.count)
            {
                verified = snapshot->acknowledgement.verified[selected];
                message = snapshot->acknowledgement.message[selected];
            }
            const char *state = !snapshot->acknowledged ? "Acknowledgement unavailable"
                                : verified > 0          ? "Verified by adapter"
                                : verified == 0 ? "Not verified by adapter"
                                                : "Accepted; individual verification not supplied";
            char *text = g_strdup_printf("%s%s%s", state, message[0] != '\0' ? ": " : "", message);
            GtkWidget *label = gtk_label_new(text);
            gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
            gtk_label_set_wrap(GTK_LABEL(label), TRUE);
            gtk_box_append(GTK_BOX(panel->rows), label);
            g_free(text);
            ++selected;
        }
    }
    gtk_widget_set_sensitive(panel->apply_button, snapshot->catalog.count != 0U);
    gtk_label_set_text(GTK_LABEL(panel->status),
                       snapshot->catalog.count == 0U
                           ? "This adapter did not advertise exception filters."
                           : "Choose the exception filters, then Apply. Refresh discards unapplied "
                             "checkbox changes.");
}
static UmiStatus ExceptionPanelRead(ExceptionPanel *panel)
{
    UmiDebugExceptionSnapshot *snapshot = g_new0(UmiDebugExceptionSnapshot, 1);
    UmiStatus status = panel->read(panel->context, snapshot);
    /* Even trusted host code can return an incomplete snapshot. Bound row
     * creation before indexing fixed arrays or passing strings to GTK. */
    if (status == UMI_STATUS_OK &&
        (snapshot->catalog.count > UMI_DEBUG_EXCEPTION_FILTER_LIMIT ||
         snapshot->catalog.count != snapshot->selection.count ||
         snapshot->acknowledgement.count > UMI_DEBUG_EXCEPTION_FILTER_LIMIT))
        status = UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot->catalog.count; ++i)
    {
        if (memchr(snapshot->catalog.items[i].label, '\0',
                   sizeof snapshot->catalog.items[i].label) == NULL ||
            memchr(snapshot->catalog.items[i].description, '\0',
                   sizeof snapshot->catalog.items[i].description) == NULL ||
            memchr(snapshot->acknowledgement.message[i], '\0',
                   sizeof snapshot->acknowledgement.message[i]) == NULL ||
            !g_utf8_validate(snapshot->catalog.items[i].label, -1, NULL) ||
            !g_utf8_validate(snapshot->catalog.items[i].description, -1, NULL) ||
            !g_utf8_validate(snapshot->acknowledgement.message[i], -1, NULL))
            status = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (status == UMI_STATUS_OK)
        ExceptionPanelRender(panel, snapshot);
    else
    {
        ExceptionPanelClear(panel);
        gtk_label_set_text(GTK_LABEL(panel->status),
                           status == UMI_STATUS_NOT_FOUND
                               ? "Start or attach a debugger, then choose Refresh."
                               : umi_status_text(status));
    }
    g_free(snapshot);
    return status;
}
static void ExceptionPanelRefresh(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = data;
    ExceptionPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-exception-panel");
    if (panel->pending || !gtk_widget_get_mapped(root))
        return;
    /* A host callback can close its window. Retain the root until the callback
     * and presentation update have both returned; no stale workbench is retained. */
    g_object_ref(root);
    panel->pending = TRUE;
    (void)ExceptionPanelRead(panel);
    panel->pending = FALSE;
    g_object_unref(root);
}
static void ExceptionPanelApply(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = data;
    ExceptionPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-exception-panel");
    if (panel->pending || !panel->captured || !gtk_widget_get_mapped(root))
        return;
    g_object_ref(root);
    panel->pending = TRUE;
    UmiDebugExceptionSelection selection = panel->snapshot.selection;
    for (size_t i = 0U; i < selection.count; ++i)
        selection.enabled[i] = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->checks[i]));
    UmiStatus status = panel->apply(panel->context, &selection);
    /* Always reread after Apply, including failures. A timeout may have changed
     * the adapter, and a fresh revision prevents accidental replay of old intent. */
    (void)ExceptionPanelRead(panel);
    if (status != UMI_STATUS_OK)
        gtk_label_set_text(GTK_LABEL(panel->status), umi_status_text(status));
    panel->pending = FALSE;
    g_object_unref(root);
}
GtkWidget *UmiGtk4DebugExceptionFiltersCreate(UmiGtk4ExceptionFiltersRead read,
                                              UmiGtk4ExceptionFiltersApply apply, void *context,
                                              GDestroyNotify destroy)
{
    if (read == NULL || apply == NULL)
        return NULL;
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    ExceptionPanel *panel = g_new0(ExceptionPanel, 1);
    panel->root = root;
    panel->read = read;
    panel->apply = apply;
    panel->context = context;
    panel->destroy = destroy;
    panel->rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    panel->status = gtk_label_new("Start or attach a debugger, then choose Refresh.");
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *refresh = gtk_button_new_with_label("Refresh exception filters");
    panel->apply_button = gtk_button_new_with_label("Apply exception filters");
    gtk_widget_set_sensitive(panel->apply_button, FALSE);
    gtk_box_append(GTK_BOX(actions), refresh);
    gtk_box_append(GTK_BOX(actions), panel->apply_button);
    gtk_box_append(GTK_BOX(root), actions);
    gtk_box_append(GTK_BOX(root), panel->status);
    gtk_box_append(GTK_BOX(root), panel->rows);
    (void)umi_gtk4_automation_tag_widget(root, "debug.exceptions.panel");
    (void)umi_gtk4_automation_tag_widget(refresh, "debug.exceptions.refresh");
    (void)umi_gtk4_automation_tag_widget(panel->apply_button, "debug.exceptions.apply");
    (void)umi_gtk4_automation_tag_widget(panel->status, "debug.exceptions.status");
    g_object_set_data_full(G_OBJECT(root), "umicom-exception-panel", panel, ExceptionPanelFree);
    g_signal_connect_object(refresh, "clicked", G_CALLBACK(ExceptionPanelRefresh), root, 0);
    g_signal_connect_object(panel->apply_button, "clicked", G_CALLBACK(ExceptionPanelApply), root,
                            0);
    return root;
}

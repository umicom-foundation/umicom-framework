/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_exception_information.c
 * PURPOSE: Show nested exception descriptions without evaluating or executing adapter-provided text.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/debug_exception_information.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>
typedef struct ExceptionPanel
{
    GtkWidget *root, *status, *details;
    UmiGtk4DebugExceptionInformationQuery query;
    void *context;
    GDestroyNotify destroy;
    gboolean pending;
} ExceptionPanel;
static void ExceptionPanelFree(gpointer data)
{
    ExceptionPanel *panel = data;
    if (panel->destroy != NULL)
        panel->destroy(panel->context);
    g_free(panel);
}
static bool ExceptionPanelText(const char *text, size_t capacity)
{
    return memchr(text, '\0', capacity) != NULL && g_utf8_validate(text, -1, NULL);
}
static bool ExceptionPanelValid(const UmiDebugExceptionTarget *target,
                                const UmiDebugExceptionInformation *information)
{
    if (target->connection.generation == 0U || target->thread_id == 0U ||
        !ExceptionPanelText(target->connection.session_id, sizeof target->connection.session_id) ||
        target->connection.session_id[0] == '\0' ||
        information->count > UMI_DEBUG_EXCEPTION_DETAIL_LIMIT ||
        !ExceptionPanelText(information->exception_id, sizeof information->exception_id) ||
        !ExceptionPanelText(information->description, sizeof information->description) ||
        !ExceptionPanelText(information->break_mode, sizeof information->break_mode))
        return false;
    for (size_t i = 0U; i < information->count; ++i)
    {
        const UmiDebugExceptionDetail *detail = &information->details[i];
        if (detail->depth >= UMI_DEBUG_EXCEPTION_DEPTH_LIMIT)
            return false;
        if (i == 0U)
        {
            if (detail->parent != SIZE_MAX || detail->depth != 0U)
                return false;
        }
        else if (detail->parent >= i ||
                 detail->depth != information->details[detail->parent].depth + 1U)
            return false;
#define EXCEPTION_VALID(field)                                                                     \
    if (!ExceptionPanelText(detail->field, sizeof detail->field))                                  \
    return false
        EXCEPTION_VALID(message);
        EXCEPTION_VALID(type_name);
        EXCEPTION_VALID(full_type_name);
        EXCEPTION_VALID(evaluate_name);
        EXCEPTION_VALID(stack_trace);
#undef EXCEPTION_VALID
    }
    return true;
}
static void ExceptionPanelClear(ExceptionPanel *panel)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->details)) != NULL)
        gtk_box_remove(GTK_BOX(panel->details), child);
}
static void ExceptionField(GtkWidget *parent, const char *name, const char *value)
{
    if (value[0] == '\0')
        return;
    char *text = g_strdup_printf("%s: %s", name, value);
    GtkWidget *label = gtk_label_new(text);
    g_free(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_box_append(GTK_BOX(parent), label);
}
static void ExceptionPanelRender(ExceptionPanel *panel, const UmiDebugExceptionTarget *target,
                                 const UmiDebugExceptionInformation *information)
{
    ExceptionField(panel->details, "Exception", information->exception_id);
    ExceptionField(panel->details, "Description", information->description);
    ExceptionField(panel->details, "Break mode", information->break_mode);
    GtkWidget *parents[UMI_DEBUG_EXCEPTION_DETAIL_LIMIT] = {0};
    for (size_t i = 0U; i < information->count; ++i)
    {
        const UmiDebugExceptionDetail *detail = &information->details[i];
        GtkWidget *expander =
            gtk_expander_new(detail->type_name[0] ? detail->type_name : "Exception details");
        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        parents[i] = box;
        gtk_expander_set_child(GTK_EXPANDER(expander), box);
        gtk_box_append(
            GTK_BOX(detail->parent == SIZE_MAX ? panel->details : parents[detail->parent]),
            expander);
        gtk_expander_set_expanded(GTK_EXPANDER(expander), i == 0U);
        ExceptionField(box, "Message", detail->message);
        ExceptionField(box, "Full type", detail->full_type_name);
        ExceptionField(box, "Expression supplied by adapter (not evaluated)",
                       detail->evaluate_name);
        if (detail->stack_trace[0] != '\0')
        {
            /* A stack trace can contain source-looking text, links or markup.
             * A read-only text buffer preserves it without interpreting any of it. */
            GtkWidget *view = gtk_text_view_new(), *scroll = gtk_scrolled_window_new();
            gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
            gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)),
                                     detail->stack_trace, -1);
            gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
            gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 120);
            char tag[80];
            g_snprintf(tag, sizeof tag, "debug.exception.trace.%zu", i);
            (void)umi_gtk4_automation_tag_widget(view, tag);
            gtk_box_append(GTK_BOX(box), scroll);
        }
    }
    char *status = g_strdup_printf("Captured from thread %u, inspection revision %llu; %zu detail "
                                   "records. Refresh after another stop.",
                                   target->thread_id, (unsigned long long)target->revision,
                                   information->count);
    gtk_label_set_text(GTK_LABEL(panel->status), status);
    g_free(status);
}
static void ExceptionPanelRefresh(GtkButton *button, gpointer data)
{
    (void)button;
    ExceptionPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-exception-information");
    if (panel->pending || !gtk_widget_get_mapped(panel->root))
        return;
    GtkWidget *root = g_object_ref(panel->root);
    panel->pending = TRUE;
    ExceptionPanelClear(panel);
    UmiDebugExceptionInformation *information = g_new0(UmiDebugExceptionInformation, 1);
    UmiDebugExceptionTarget target = {0};
    UmiStatus status = panel->query(panel->context, &target, information);
    if (status == UMI_STATUS_OK && !ExceptionPanelValid(&target, information))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        ExceptionPanelRender(panel, &target, information);
    else
        gtk_label_set_text(GTK_LABEL(panel->status),
                           status == UMI_STATUS_NOT_IMPLEMENTED
                               ? "This adapter did not advertise exception information."
                           : status == UMI_STATUS_NOT_FOUND
                               ? "Pause at an exception before requesting its details."
                               : umi_status_text(status));
    g_free(information);
    panel->pending = FALSE;
    g_object_unref(root);
}
GtkWidget *UmiGtk4DebugExceptionInformationCreate(UmiGtk4DebugExceptionInformationQuery query,
                                                  void *context, GDestroyNotify destroy)
{
    if (query == NULL)
        return NULL;
    ExceptionPanel *panel = g_new0(ExceptionPanel, 1);
    panel->query = query;
    panel->context = context;
    panel->destroy = destroy;
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->details = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    panel->status = gtk_label_new("Pause at an exception, then choose Refresh exception.");
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    GtkWidget *refresh = gtk_button_new_with_label("Refresh exception");
    GtkWidget *explanation = gtk_label_new("Captured exception details are descriptive. Stack "
                                           "traces and supplied expressions are not executed.");
    gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
    gtk_label_set_xalign(GTK_LABEL(explanation), 0.0F);
    gtk_box_append(GTK_BOX(panel->root), explanation);
    gtk_box_append(GTK_BOX(panel->root), refresh);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    gtk_box_append(GTK_BOX(panel->root), panel->details);
    (void)umi_gtk4_automation_tag_widget(panel->root, "debug.exception.panel");
    (void)umi_gtk4_automation_tag_widget(refresh, "debug.exception.refresh");
    (void)umi_gtk4_automation_tag_widget(panel->status, "debug.exception.status");
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-exception-information", panel,
                           ExceptionPanelFree);
    g_signal_connect_object(refresh, "clicked", G_CALLBACK(ExceptionPanelRefresh), panel->root, 0);
    return panel->root;
}

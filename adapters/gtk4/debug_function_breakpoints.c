/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_function_breakpoints.c
 * PURPOSE: Edit complete function-breakpoint drafts while retaining session identity and host authority.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/debug_function_breakpoints.h"
#include "umicom/base/text.h"
#include "umicom/ui/gtk4/automation.h"
#include <string.h>
typedef struct FunctionPanel
{
    GtkWidget *root, *rows, *status, *apply_button, *add_button;
    GtkWidget *enabled[UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT];
    GtkWidget *names[UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT];
    GtkWidget *conditions[UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT];
    GtkWidget *hits[UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT];
    GtkWidget *verification_labels[UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT];
    UmiDebugFunctionSnapshot snapshot;
    UmiGtk4FunctionBreakpointsRead read;
    UmiGtk4FunctionBreakpointsApply apply;
    void *context;
    GDestroyNotify destroy;
    gboolean captured, pending;
} FunctionPanel;
static void FunctionPanelRender(FunctionPanel *panel);
static void FunctionPanelFree(gpointer data)
{
    FunctionPanel *panel = data;
    if (panel->destroy != NULL)
        panel->destroy(panel->context);
    g_free(panel);
}
static UmiStatus FunctionPanelDraft(FunctionPanel *panel, UmiDebugFunctionDraft *out)
{
    UmiDebugFunctionDraft *draft = g_new(UmiDebugFunctionDraft, 1);
    *draft = panel->snapshot.draft;
    UmiStatus status = UMI_STATUS_OK;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < draft->count; ++i)
    {
        UmiDebugFunctionEntry *entry = &draft->entries[i];
        entry->enabled = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->enabled[i]));
        status = umi_text_copy(entry->breakpoint.name, sizeof entry->breakpoint.name,
                               gtk_editable_get_text(GTK_EDITABLE(panel->names[i])));
        if (status == UMI_STATUS_OK)
            status = umi_text_copy(entry->breakpoint.condition, sizeof entry->breakpoint.condition,
                                   gtk_editable_get_text(GTK_EDITABLE(panel->conditions[i])));
        if (status == UMI_STATUS_OK)
            status = umi_text_copy(entry->breakpoint.hit_condition,
                                   sizeof entry->breakpoint.hit_condition,
                                   gtk_editable_get_text(GTK_EDITABLE(panel->hits[i])));
    }
    if (status == UMI_STATUS_OK)
        *out = *draft;
    g_free(draft);
    return status;
}
static void FunctionPanelClearRows(FunctionPanel *panel)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(panel->rows)) != NULL)
        gtk_box_remove(GTK_BOX(panel->rows), child);
    memset(panel->enabled, 0, sizeof panel->enabled);
    memset(panel->names, 0, sizeof panel->names);
    memset(panel->conditions, 0, sizeof panel->conditions);
    memset(panel->hits, 0, sizeof panel->hits);
    memset(panel->verification_labels, 0, sizeof panel->verification_labels);
}
static void FunctionPanelRemove(GtkButton *button, gpointer data)
{
    FunctionPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-function-panel");
    if (panel->pending || !panel->captured || !gtk_widget_get_mapped(panel->root))
        return;
    size_t index =
        (size_t)GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(button), "umicom-function-row"));
    if (index >= panel->snapshot.draft.count)
        return;
    UmiStatus status = FunctionPanelDraft(panel, &panel->snapshot.draft);
    if (status != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->status), umi_status_text(status));
        return;
    }
    /* Removing a presentation row does not clear a debugger breakpoint. The
     * complete resulting draft is sent only after the user chooses Apply. */
    for (size_t i = index + 1U; i < panel->snapshot.draft.count; ++i)
        panel->snapshot.draft.entries[i - 1U] = panel->snapshot.draft.entries[i];
    --panel->snapshot.draft.count;
    memset(&panel->snapshot.draft.entries[panel->snapshot.draft.count], 0,
           sizeof panel->snapshot.draft.entries[0]);
    panel->snapshot.acknowledged = 0;
    FunctionPanelRender(panel);
}
static GtkWidget *FunctionEntry(const char *text, const char *placeholder)
{
    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_editable_set_text(GTK_EDITABLE(entry), text);
    gtk_widget_set_hexpand(entry, TRUE);
    return entry;
}
static void FunctionPanelTag(GtkWidget *widget, size_t row, const char *field)
{
    char tag[96];
    g_snprintf(tag, sizeof tag, "debug.functions.%zu.%s", row, field);
    (void)umi_gtk4_automation_tag_widget(widget, tag);
}
/* Changing a field invalidates the visible association with a previous reply.
 * Keep editor widgets intact so the caret and unsubmitted text are preserved. */
static void FunctionPanelEdited(GtkWidget *widget, gpointer data)
{
    (void)widget;
    FunctionPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-function-panel");
    panel->snapshot.acknowledged = 0;
    for (size_t i = 0U; i < panel->snapshot.draft.count; ++i)
        if (panel->verification_labels[i] != NULL)
            gtk_label_set_text(GTK_LABEL(panel->verification_labels[i]),
                               "Unapplied edit; verification will follow Apply.");
}
static void FunctionPanelRender(FunctionPanel *panel)
{
    FunctionPanelClearRows(panel);
    size_t verification = 0U;
    for (size_t i = 0U; i < panel->snapshot.draft.count; ++i)
    {
        UmiDebugFunctionEntry *entry = &panel->snapshot.draft.entries[i];
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        GtkWidget *top = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
        panel->enabled[i] = gtk_check_button_new_with_label("Enabled");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->enabled[i]), entry->enabled);
        panel->names[i] = FunctionEntry(entry->breakpoint.name, "Function name, for example main");
        GtkWidget *remove = gtk_button_new_with_label("Remove");
        g_object_set_data(G_OBJECT(remove), "umicom-function-row", GUINT_TO_POINTER((guint)i));
        g_signal_connect_object(remove, "clicked", G_CALLBACK(FunctionPanelRemove), panel->root, 0);
        gtk_box_append(GTK_BOX(top), panel->enabled[i]);
        gtk_box_append(GTK_BOX(top), panel->names[i]);
        gtk_box_append(GTK_BOX(top), remove);
        gtk_box_append(GTK_BOX(row), top);
        panel->conditions[i] =
            FunctionEntry(entry->breakpoint.condition, "Optional debugger condition");
        panel->hits[i] = FunctionEntry(entry->breakpoint.hit_condition, "Optional hit condition");
        gtk_widget_set_sensitive(panel->conditions[i], panel->snapshot.conditions_supported);
        gtk_widget_set_sensitive(panel->hits[i], panel->snapshot.hit_conditions_supported);
        gtk_box_append(GTK_BOX(row), panel->conditions[i]);
        gtk_box_append(GTK_BOX(row), panel->hits[i]);
        FunctionPanelTag(panel->names[i], i, "name");
        FunctionPanelTag(panel->enabled[i], i, "enabled");
        FunctionPanelTag(panel->conditions[i], i, "condition");
        FunctionPanelTag(panel->hits[i], i, "hit");
        FunctionPanelTag(remove, i, "remove");
        const char *state =
            entry->enabled ? "Not acknowledged for this draft" : "Disabled in this draft";
        const char *message = "";
        if (entry->enabled)
        {
            if (panel->snapshot.acknowledged && verification < panel->snapshot.reply.count)
            {
                UmiDebugFunctionVerification *reply = &panel->snapshot.reply.entries[verification];
                state = reply->verified > 0 ? "Last Apply: verified by adapter"
                                            : "Last Apply: not verified by adapter";
                message = reply->message;
            }
            ++verification;
        }
        char *text = g_strdup_printf("%s%s%s", state, message[0] != '\0' ? ": " : "", message);
        GtkWidget *label = gtk_label_new(text);
        g_free(text);
        gtk_label_set_wrap(GTK_LABEL(label), TRUE);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
        panel->verification_labels[i] = label;
        gtk_box_append(GTK_BOX(row), label);
        gtk_box_append(GTK_BOX(panel->rows), row);
        g_signal_connect_object(panel->names[i], "changed", G_CALLBACK(FunctionPanelEdited),
                                panel->root, 0);
        g_signal_connect_object(panel->conditions[i], "changed", G_CALLBACK(FunctionPanelEdited),
                                panel->root, 0);
        g_signal_connect_object(panel->hits[i], "changed", G_CALLBACK(FunctionPanelEdited),
                                panel->root, 0);
        g_signal_connect_object(panel->enabled[i], "toggled", G_CALLBACK(FunctionPanelEdited),
                                panel->root, 0);
    }
    gtk_widget_set_sensitive(panel->apply_button, panel->captured && panel->snapshot.supported);
    gtk_widget_set_sensitive(panel->add_button,
                             panel->captured && panel->snapshot.supported &&
                                 panel->snapshot.draft.count < UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT);
}
static UmiStatus FunctionPanelRead(FunctionPanel *panel)
{
    UmiDebugFunctionSnapshot *snapshot = g_new0(UmiDebugFunctionSnapshot, 1);
    UmiStatus status = panel->read(panel->context, snapshot);
    if (status == UMI_STATUS_OK)
        status = UmiDebugFunctionDraftValidate(&snapshot->draft);
    if (status == UMI_STATUS_OK && snapshot->reply.count > UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT)
        status = UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot->reply.count; ++i)
        if (memchr(snapshot->reply.entries[i].message, '\0',
                   sizeof snapshot->reply.entries[i].message) == NULL ||
            !g_utf8_validate(snapshot->reply.entries[i].message, -1, NULL))
            status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        panel->snapshot = *snapshot;
        panel->captured = TRUE;
        FunctionPanelRender(panel);
        gtk_label_set_text(GTK_LABEL(panel->status),
                           snapshot->supported
                               ? "Edit function names, then Apply the complete set. Refresh "
                                 "discards unapplied edits."
                               : "This adapter did not advertise function breakpoints.");
    }
    else
    {
        panel->captured = FALSE;
        FunctionPanelClearRows(panel);
        gtk_widget_set_sensitive(panel->apply_button, FALSE);
        gtk_widget_set_sensitive(panel->add_button, FALSE);
        gtk_label_set_text(GTK_LABEL(panel->status),
                           status == UMI_STATUS_NOT_FOUND
                               ? "Start or attach a debugger, then choose Refresh."
                               : umi_status_text(status));
    }
    g_free(snapshot);
    return status;
}
static void FunctionPanelRefresh(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = data;
    FunctionPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-function-panel");
    if (panel->pending || !gtk_widget_get_mapped(root))
        return;
    g_object_ref(root);
    panel->pending = TRUE;
    (void)FunctionPanelRead(panel);
    panel->pending = FALSE;
    g_object_unref(root);
}
static void FunctionPanelAdd(GtkButton *button, gpointer data)
{
    (void)button;
    FunctionPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-function-panel");
    if (panel->pending || !panel->captured || !panel->snapshot.supported ||
        !gtk_widget_get_mapped(panel->root) ||
        panel->snapshot.draft.count >= UMI_DEBUG_FUNCTION_BREAKPOINT_LIMIT)
        return;
    UmiStatus status = FunctionPanelDraft(panel, &panel->snapshot.draft);
    if (status != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->status), umi_status_text(status));
        return;
    }
    UmiDebugFunctionEntry *entry = &panel->snapshot.draft.entries[panel->snapshot.draft.count++];
    memset(entry, 0, sizeof *entry);
    entry->enabled = 1;
    panel->snapshot.acknowledged = 0;
    FunctionPanelRender(panel);
}
static void FunctionPanelApply(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = data;
    FunctionPanel *panel = g_object_get_data(G_OBJECT(root), "umicom-function-panel");
    if (panel->pending || !panel->captured || !panel->snapshot.supported ||
        !gtk_widget_get_mapped(root))
        return;
    UmiDebugFunctionDraft *draft = g_new(UmiDebugFunctionDraft, 1);
    UmiStatus status = FunctionPanelDraft(panel, draft);
    if (status == UMI_STATUS_OK)
        status = UmiDebugFunctionDraftValidate(draft);
    if (status != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(panel->status), umi_status_text(status));
        g_free(draft);
        return;
    }
    g_object_ref(root);
    panel->pending = TRUE;
    status = panel->apply(panel->context, draft);
    (void)FunctionPanelRead(panel);
    if (status != UMI_STATUS_OK)
        gtk_label_set_text(GTK_LABEL(panel->status), umi_status_text(status));
    panel->pending = FALSE;
    g_free(draft);
    g_object_unref(root);
}
GtkWidget *UmiGtk4DebugFunctionBreakpointsCreate(UmiGtk4FunctionBreakpointsRead read,
                                                 UmiGtk4FunctionBreakpointsApply apply,
                                                 void *context, GDestroyNotify destroy)
{
    if (read == NULL || apply == NULL)
        return NULL;
    FunctionPanel *panel = g_new0(FunctionPanel, 1);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    panel->read = read;
    panel->apply = apply;
    panel->context = context;
    panel->destroy = destroy;
    panel->status = gtk_label_new("Start or attach a debugger, then choose Refresh.");
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    GtkWidget *explanation =
        gtk_label_new("Conditions are debugger expressions and may affect the target. Apply sends "
                      "only enabled rows. These choices last for this session.");
    gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
    gtk_label_set_xalign(GTK_LABEL(explanation), 0.0F);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *refresh = gtk_button_new_with_label("Refresh function breakpoints");
    panel->add_button = gtk_button_new_with_label("Add function");
    panel->apply_button = gtk_button_new_with_label("Apply function breakpoints");
    gtk_widget_set_sensitive(panel->add_button, FALSE);
    gtk_widget_set_sensitive(panel->apply_button, FALSE);
    gtk_box_append(GTK_BOX(actions), refresh);
    gtk_box_append(GTK_BOX(actions), panel->add_button);
    gtk_box_append(GTK_BOX(actions), panel->apply_button);
    gtk_box_append(GTK_BOX(panel->root), explanation);
    gtk_box_append(GTK_BOX(panel->root), actions);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    gtk_box_append(GTK_BOX(panel->root), panel->rows);
    (void)umi_gtk4_automation_tag_widget(panel->root, "debug.functions.panel");
    (void)umi_gtk4_automation_tag_widget(refresh, "debug.functions.refresh");
    (void)umi_gtk4_automation_tag_widget(panel->add_button, "debug.functions.add");
    (void)umi_gtk4_automation_tag_widget(panel->apply_button, "debug.functions.apply");
    (void)umi_gtk4_automation_tag_widget(panel->status, "debug.functions.status");
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-function-panel", panel,
                           FunctionPanelFree);
    g_signal_connect_object(refresh, "clicked", G_CALLBACK(FunctionPanelRefresh), panel->root, 0);
    g_signal_connect_object(panel->add_button, "clicked", G_CALLBACK(FunctionPanelAdd), panel->root,
                            0);
    g_signal_connect_object(panel->apply_button, "clicked", G_CALLBACK(FunctionPanelApply),
                            panel->root, 0);
    return panel->root;
}

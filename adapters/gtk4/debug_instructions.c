/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/debug_instructions.c
 * PURPOSE: Display bounded instruction pages as read-only text while guarding callbacks and changed drafts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/debug_instructions.h"
#include "umicom/ui/gtk4/automation.h"
#include <limits.h>
#include <stdint.h>
#include <string.h>
typedef struct InstructionPanel
{
    GtkWidget *root, *status, *output, *bytes, *offset, *count, *symbols, *read, *earlier, *later;
    UmiGtk4InstructionCapture capture;
    UmiGtk4InstructionRead query;
    void *context;
    GDestroyNotify destroy;
    UmiDebugInstructionContext target;
    uint64_t edit_revision;
    gboolean captured, pending, editing;
} InstructionPanel;
/* The panel owns the callback context after construction and releases it once. */
static void InstructionPanelFree(gpointer data)
{
    InstructionPanel *panel = data;
    if (panel->destroy != NULL)
        panel->destroy(panel->context);
    g_free(panel);
}
/* Host callbacks cross an extension boundary; bound each string before GTK reads it. */
static gboolean InstructionText(const char *text, size_t capacity)
{
    return memchr(text, 0, capacity) != NULL && g_utf8_validate(text, -1, NULL);
}
/* A copied context describes one connection and one paused frame, including frame zero. */
static gboolean InstructionContextValid(const UmiDebugInstructionContext *context)
{
    return context->connection.generation != 0U && context->thread_id != 0U &&
           context->thread_id <= INT32_MAX && context->frame_id <= INT32_MAX &&
           InstructionText(context->connection.session_id, sizeof context->connection.session_id) &&
           context->connection.session_id[0] != '\0' &&
           InstructionText(context->reference, sizeof context->reference) &&
           context->reference[0] != '\0';
}
/* Reject malformed callback output before walking rows or handing strings to GTK. */
static gboolean InstructionPageValid(const UmiDebugInstructionPage *page, uint32_t count)
{
    if (page->count != count || count == 0U || count > UMI_DEBUG_INSTRUCTION_PAGE_LIMIT)
        return FALSE;
    for (size_t i = 0U; i < page->count; ++i)
    {
        const UmiDebugInstructionRecord *row = &page->items[i];
#define INSTRUCTION_VALID(field)                                                                   \
    if (!InstructionText(row->field, sizeof row->field))                                           \
    return FALSE
        INSTRUCTION_VALID(address);
        INSTRUCTION_VALID(bytes);
        INSTRUCTION_VALID(text);
        INSTRUCTION_VALID(symbol);
        INSTRUCTION_VALID(presentation_hint);
        INSTRUCTION_VALID(source.name);
        INSTRUCTION_VALID(source.path);
#undef INSTRUCTION_VALID
        if (row->address[0] == '\0' || row->source.source_reference > INT32_MAX ||
            row->line > UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT ||
            row->column > UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT ||
            row->end_line > UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT ||
            row->end_column > UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT)
            return FALSE;
    }
    return TRUE;
}
/* Clear stale rows as soon as their request settings or validity change. */
static void InstructionPageClear(InstructionPanel *panel)
{
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->output)), "", 0);
}
/* Paging needs a capture and cannot overlap another owner-thread request. */
static void InstructionControls(InstructionPanel *panel)
{
    gtk_widget_set_sensitive(panel->read, panel->captured && !panel->pending);
    gtk_widget_set_sensitive(panel->earlier, panel->captured && !panel->pending);
    gtk_widget_set_sensitive(panel->later, panel->captured && !panel->pending);
}
/* Track nested edits so a callback cannot publish under a different request draft. */
static void InstructionChanged(GtkWidget *widget, gpointer data)
{
    (void)widget;
    InstructionPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-instructions");
    if (panel->editing)
        return;
    if (panel->edit_revision != UINT64_MAX)
        ++panel->edit_revision;
    else
        panel->captured = FALSE;
    InstructionPageClear(panel);
    InstructionControls(panel);
    gtk_label_set_text(
        GTK_LABEL(panel->status),
        "Request settings changed. Choose Read page, or Refresh for the current frame.");
}
/* Render plain text only; source paths and symbols never become executable actions. */
static void InstructionRender(InstructionPanel *panel, const UmiDebugInstructionPage *page,
                              int64_t byte_offset, int64_t instruction_offset)
{
    GString *text = g_string_new(NULL);
    for (size_t i = 0U; i < page->count; ++i)
    {
        const UmiDebugInstructionRecord *row = &page->items[i];
        g_string_append_printf(text, "%s%s  %s  %s\n",
                               strcmp(row->presentation_hint, "invalid") == 0 ? "[unavailable] "
                                                                              : "",
                               row->address, row->bytes, row->text);
        if (row->symbol[0] != '\0')
            g_string_append_printf(text, "    Symbol: %s\n", row->symbol);
        if (row->presentation_hint[0] != '\0' && strcmp(row->presentation_hint, "invalid") &&
            strcmp(row->presentation_hint, "normal"))
            g_string_append_printf(text, "    Presentation: %s\n", row->presentation_hint);
        if (row->has_source)
        {
            g_string_append_printf(text, "    Source: %s",
                                   row->source.path[0] ? row->source.path : row->source.name);
            if (row->source.source_reference)
                g_string_append_printf(text, " (reference %llu)",
                                       (unsigned long long)row->source.source_reference);
            if (row->line_known)
                g_string_append_printf(text, "  line %llu", (unsigned long long)row->line);
            if (row->column_known)
                g_string_append_printf(text, ", column %llu", (unsigned long long)row->column);
            if (row->end_line_known)
                g_string_append_printf(text, "; end line %llu", (unsigned long long)row->end_line);
            if (row->end_column_known)
                g_string_append_printf(text, ", end column %llu",
                                       (unsigned long long)row->end_column);
            g_string_append_c(text, '\n');
        }
    }
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->output)), text->str,
                             (int)text->len);
    g_string_free(text, TRUE);
    char *summary = g_strdup_printf(
        "Captured thread %u, frame %u; reference %s; byte offset %lld, instruction offset %lld. "
        "%zu instructions. Refresh after the debugger changes.",
        panel->target.thread_id, panel->target.frame_id, panel->target.reference,
        (long long)byte_offset, (long long)instruction_offset, page->count);
    gtk_label_set_text(GTK_LABEL(panel->status), summary);
    g_free(summary);
}
/* Hold the panel while callbacks run: a callback may close its containing window. */
static void InstructionAction(GtkButton *button, gpointer data)
{
    InstructionPanel *panel = g_object_get_data(G_OBJECT(data), "umicom-instructions");
    if (panel->edit_revision == UINT64_MAX)
        return;
    if (panel->pending || !gtk_widget_get_mapped(GTK_WIDGET(button)) ||
        !gtk_widget_get_sensitive(GTK_WIDGET(button)))
        return;
    GtkWidget *root = g_object_ref(panel->root);
    gboolean refresh = GTK_WIDGET(button) != panel->read && GTK_WIDGET(button) != panel->earlier &&
                       GTK_WIDGET(button) != panel->later;
    UmiStatus status = UMI_STATUS_OK;
    int64_t bytes = 0, offset = 0;
    uint32_t count = (uint32_t)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->count));
    int symbols = gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->symbols));
    if (refresh)
    {
        panel->editing = TRUE;
        gtk_editable_set_text(GTK_EDITABLE(panel->bytes), "0");
        gtk_editable_set_text(GTK_EDITABLE(panel->offset), "0");
        panel->editing = FALSE;
    }
    status =
        UmiDebugInstructionOffsetRead(gtk_editable_get_text(GTK_EDITABLE(panel->bytes)), &bytes);
    if (status == UMI_STATUS_OK)
        status = UmiDebugInstructionOffsetRead(gtk_editable_get_text(GTK_EDITABLE(panel->offset)),
                                               &offset);
    if (status == UMI_STATUS_OK &&
        (GTK_WIDGET(button) == panel->earlier || GTK_WIDGET(button) == panel->later))
    {
        int64_t change = GTK_WIDGET(button) == panel->earlier ? -(int64_t)count : (int64_t)count;
        if ((change > 0 && offset > UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT - change) ||
            (change < 0 && offset < -UMI_DEBUG_INSTRUCTION_OFFSET_LIMIT - change))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
        {
            offset += change;
            char value[32];
            g_snprintf(value, sizeof value, "%lld", (long long)offset);
            panel->editing = TRUE;
            gtk_editable_set_text(GTK_EDITABLE(panel->offset), value);
            panel->editing = FALSE;
        }
    }
    InstructionPageClear(panel);
    panel->pending = TRUE;
    InstructionControls(panel);
    uint64_t draft = panel->edit_revision;
    if (status == UMI_STATUS_OK && refresh)
    {
        panel->captured = FALSE;
        status = panel->capture(panel->context, &panel->target);
        if (status == UMI_STATUS_OK && !InstructionContextValid(&panel->target))
            status = UMI_STATUS_INVALID_ARGUMENT;
        if (status == UMI_STATUS_OK)
            panel->captured = TRUE;
    }
    if (status == UMI_STATUS_OK &&
        (panel->edit_revision != draft || !panel->captured || !gtk_widget_get_mapped(root)))
        status = UMI_STATUS_CANCELLED;
    UmiDebugInstructionPage *page = g_new0(UmiDebugInstructionPage, 1);
    if (status == UMI_STATUS_OK)
        status = panel->query(panel->context, &panel->target, bytes, offset, count, symbols, page);
    /* A host callback may dispatch nested UI events. Do not display a reply
     * under edited settings or after the window has closed. */
    if (status == UMI_STATUS_OK && (panel->edit_revision != draft || !gtk_widget_get_mapped(root)))
        status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK && !InstructionPageValid(page, count))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
        InstructionRender(panel, page, bytes, offset);
    else
    {
        panel->captured = FALSE;
        gtk_label_set_text(GTK_LABEL(panel->status),
                           status == UMI_STATUS_NOT_IMPLEMENTED
                               ? "This adapter does not advertise instruction disassembly."
                           : status == UMI_STATUS_NOT_FOUND
                               ? "The selected frame has no available instruction reference."
                               : umi_status_text(status));
    }
    g_free(page);
    panel->pending = FALSE;
    InstructionControls(panel);
    g_object_unref(root);
}
/* Offset entries keep the original signed text until the shared parser accepts it. */
static GtkWidget *InstructionField(GtkWidget *parent, const char *label, const char *tag)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6), *entry = gtk_entry_new();
    GtkWidget *name = gtk_label_new(label);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_editable_set_text(GTK_EDITABLE(entry), "0");
    gtk_box_append(GTK_BOX(row), name);
    gtk_box_append(GTK_BOX(row), entry);
    gtk_box_append(GTK_BOX(parent), row);
    (void)umi_gtk4_automation_tag_widget(entry, tag);
    return entry;
}
/* Build a reusable view whose application callbacks retain project and trust policy. */
GtkWidget *UmiGtk4DebugInstructionsCreate(UmiGtk4InstructionCapture capture,
                                          UmiGtk4InstructionRead read, void *context,
                                          GDestroyNotify destroy)
{
    if (capture == NULL || read == NULL)
        return NULL;
    InstructionPanel *panel = g_new0(InstructionPanel, 1);
    panel->capture = capture;
    panel->query = read;
    panel->context = context;
    panel->destroy = destroy;
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    GtkWidget *explanation =
        gtk_label_new("Inspect machine instructions for the selected paused frame. "
                      "Offsets count bytes and instructions separately. This view does not execute "
                      "instructions or modify memory.");
    gtk_label_set_wrap(GTK_LABEL(explanation), TRUE);
    gtk_label_set_xalign(GTK_LABEL(explanation), 0.0F);
    gtk_box_append(GTK_BOX(panel->root), explanation);
    panel->bytes = InstructionField(panel->root, "Byte offset", "debug.instructions.bytes");
    panel->offset =
        InstructionField(panel->root, "Instruction offset", "debug.instructions.offset");
    GtkWidget *options = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    panel->count = gtk_spin_button_new_with_range(1, UMI_DEBUG_INSTRUCTION_PAGE_LIMIT, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->count), 32);
    panel->symbols = gtk_check_button_new_with_label("Resolve symbols");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->symbols), TRUE);
    gtk_box_append(GTK_BOX(options), gtk_label_new("Instructions per page"));
    gtk_box_append(GTK_BOX(options), panel->count);
    gtk_box_append(GTK_BOX(options), panel->symbols);
    gtk_box_append(GTK_BOX(panel->root), options);
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6),
              *refresh = gtk_button_new_with_label("Refresh instructions");
    panel->read = gtk_button_new_with_label("Read page");
    panel->earlier = gtk_button_new_with_label("Earlier instructions");
    panel->later = gtk_button_new_with_label("Later instructions");
    GtkWidget *buttons[] = {refresh, panel->read, panel->earlier, panel->later};
    const char *tags[] = {"debug.instructions.refresh", "debug.instructions.read",
                          "debug.instructions.earlier", "debug.instructions.later"};
    for (size_t i = 0U; i < 4U; ++i)
    {
        gtk_box_append(GTK_BOX(actions), buttons[i]);
        (void)umi_gtk4_automation_tag_widget(buttons[i], tags[i]);
        g_signal_connect_object(buttons[i], "clicked", G_CALLBACK(InstructionAction), panel->root,
                                0);
    }
    panel->status = gtk_label_new("Pause the debugger and choose Refresh instructions.");
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    panel->output = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->output), TRUE);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 240);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->output);
    gtk_box_append(GTK_BOX(panel->root), actions);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    gtk_box_append(GTK_BOX(panel->root), scroll);
    (void)umi_gtk4_automation_tag_widget(panel->root, "debug.instructions.panel");
    (void)umi_gtk4_automation_tag_widget(panel->output, "debug.instructions.output");
    (void)umi_gtk4_automation_tag_widget(panel->status, "debug.instructions.status");
    (void)umi_gtk4_automation_tag_widget(panel->count, "debug.instructions.count");
    (void)umi_gtk4_automation_tag_widget(panel->symbols, "debug.instructions.symbols");
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-instructions", panel,
                           InstructionPanelFree);
    g_signal_connect_object(panel->bytes, "changed", G_CALLBACK(InstructionChanged), panel->root,
                            0);
    g_signal_connect_object(panel->offset, "changed", G_CALLBACK(InstructionChanged), panel->root,
                            0);
    g_signal_connect_object(panel->count, "value-changed", G_CALLBACK(InstructionChanged),
                            panel->root, 0);
    g_signal_connect_object(panel->symbols, "toggled", G_CALLBACK(InstructionChanged), panel->root,
                            0);
    InstructionControls(panel);
    return panel->root;
}

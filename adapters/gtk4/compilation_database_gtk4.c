/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/compilation_database_gtk4.c
 * PURPOSE: Keep compiler database disk reads off the UI thread and publish only current reviewed snapshots.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/text.h"
#include "umicom/developer_project/compilation_database.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/compilation_database.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdint.h>
#include <string.h>

typedef struct CompilationRead
{
    GWeakRef panel;
    UmiCancellationToken *cancel;
    UmiCompilationDatabase *database;
    char directory[UMI_BUILD_PATH_CAPACITY], source[UMI_BUILD_PATH_CAPACITY];
    uint64_t generation;
    size_t selected;
    UmiStatus status, find_status;
} CompilationRead;
typedef struct CompilationPanel
{
    GtkWidget *panel, *directory, *source, *read, *cancel, *row, *use, *message, *details;
    GPtrArray *controls;
    GWeakRef arguments;
    UmiCompilationDatabase *database;
    CompilationRead *pending; /* Borrowed until main-context completion. */
    char reviewed_directory[UMI_BUILD_PATH_CAPACITY];
    uint64_t generation;
    bool changing, ready;
} CompilationPanel;
static CompilationPanel *CompilationPanelFrom(GtkWidget *widget)
{
    return g_object_get_data(G_OBJECT(widget), "umicom-compilation-panel");
}
static bool CompilationPanelLive(CompilationPanel *panel)
{
    if (panel == NULL || !gtk_widget_get_mapped(panel->panel))
        return false;
    GtkRoot *root = gtk_widget_get_root(panel->panel);
    return GTK_IS_WINDOW(root) && UmiGtk4WindowIsOpen(GTK_WINDOW(root));
}
static void CompilationReadFree(CompilationRead *read)
{
    g_weak_ref_clear(&read->panel);
    umi_cancellation_token_destroy(read->cancel);
    UmiCompilationDatabaseDestroy(read->database);
    g_free(read);
}
static void CompilationPanelFree(gpointer data)
{
    CompilationPanel *panel = data;
    if (panel->pending != NULL)
        umi_cancellation_token_request(panel->pending->cancel);
    UmiCompilationDatabaseDestroy(panel->database);
    g_weak_ref_clear(&panel->arguments);
    g_ptr_array_unref(panel->controls);
    g_free(panel);
}
/* An edit-away-and-back is still a new draft. At counter exhaustion reads stay
 * disabled; no old captured generation can become current again. */
static void CompilationInvalidate(CompilationPanel *panel)
{
    panel->ready = false;
    if (panel->generation != UINT64_MAX)
        ++panel->generation;
    if (panel->pending != NULL)
        umi_cancellation_token_request(panel->pending->cancel);
}
static void CompilationChanged(GtkEditable *editable, gpointer data)
{
    (void)editable;
    GtkWidget *widget = g_object_ref(GTK_WIDGET(data));
    CompilationPanel *panel = CompilationPanelFrom(widget);
    CompilationInvalidate(panel);
    gtk_widget_set_sensitive(panel->use, FALSE);
    if (CompilationPanelLive(panel) && !panel->changing)
    {
        panel->changing = true;
        gtk_label_set_text(GTK_LABEL(panel->message),
                           "Inputs changed. Read the current compiler database.");
        if (CompilationPanelLive(panel))
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->details)),
                                     "The displayed snapshot was invalidated by an input change.",
                                     -1);
        panel->changing = false;
    }
    g_object_unref(widget);
}
static void CompilationUnmapped(GtkWidget *widget, gpointer data)
{
    (void)data;
    CompilationInvalidate(CompilationPanelFrom(widget));
}
static void CompilationMapped(GtkWidget *widget, gpointer data)
{
    (void)data;
    g_object_ref(widget);
    CompilationPanel *panel = CompilationPanelFrom(widget);
    panel->changing = true;
    gtk_widget_set_sensitive(panel->use, FALSE);
    if (CompilationPanelLive(panel))
        gtk_widget_set_sensitive(panel->read, panel->pending == NULL);
    if (CompilationPanelLive(panel))
        gtk_widget_set_sensitive(panel->cancel, panel->pending != NULL);
    if (CompilationPanelLive(panel) && panel->pending == NULL)
        gtk_label_set_text(GTK_LABEL(panel->message),
                           "Choose the folder containing compile_commands.json, then Read.");
    panel->changing = false;
    g_object_unref(widget);
}
static bool CompilationAppend(GString *text, const char *value)
{
    /* Keep previews responsive even when the stored command has thousands of
     * large arguments. Omit a whole argument rather than break a UTF-8 character. */
    if (text->len + strlen(value) > 131072U)
    {
        g_string_append(text, "\n[Remaining text omitted from this preview.]\n");
        return false;
    }
    g_string_append(text, value);
    return true;
}
static void CompilationShowRow(CompilationPanel *panel)
{
    if (!panel->ready || !CompilationPanelLive(panel))
        return;
    size_t count = UmiCompilationDatabaseCount(panel->database);
    if (count == 0U)
    {
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->details)),
                                 "This database has no compiler command rows.", -1);
        return;
    }
    int number = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->row));
    if (number < 1)
        return;
    size_t index = (size_t)(number - 1);
    UmiCompilationCommand row;
    if (UmiCompilationDatabaseAt(panel->database, index, &row) != UMI_STATUS_OK)
        return;
    GString *text = g_string_new(NULL);
    g_string_append_printf(text, "Row %zu of %zu\nSource: %s\nWorking folder: %s\nOutput: %s\n\n",
                           index + 1U, count, row.source_file, row.directory,
                           row.output_file[0] != '\0' ? row.output_file : "(not recorded)");
    char *value = g_try_malloc(UMI_COMPILATION_DATABASE_TEXT_LIMIT + 1U);
    if (value == NULL)
    {
        g_string_free(text, TRUE);
        return;
    }
    if (row.has_arguments)
    {
        g_string_append(text, "Arguments (one recorded value per numbered entry):\n");
        for (size_t i = 0U; i < row.argument_count && i < 256U; ++i)
        {
            if (UmiCompilationDatabaseArgument(panel->database, index, i, value,
                                               UMI_COMPILATION_DATABASE_TEXT_LIMIT + 1U) !=
                UMI_STATUS_OK)
                break;
            g_string_append_printf(text, "%zu: ", i);
            if (!CompilationAppend(text, value))
                break;
            g_string_append_c(text, '\n');
        }
        if (row.argument_count > 256U)
            g_string_append(text, "[Only the first 256 arguments are displayed; the snapshot "
                                  "retains every argument.]\n");
    }
    if (row.has_command)
    {
        g_string_append(text, "\nRecorded command (display only):\n");
        if (UmiCompilationDatabaseCommand(panel->database, index, value,
                                          UMI_COMPILATION_DATABASE_TEXT_LIMIT + 1U) ==
            UMI_STATUS_OK)
            (void)CompilationAppend(text, value);
    }
    g_free(value);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->details)), text->str,
                             -1);
    g_string_free(text, TRUE);
}
static void CompilationRowChanged(GtkSpinButton *button, gpointer data)
{
    (void)button;
    GtkWidget *widget = g_object_ref(GTK_WIDGET(data));
    CompilationPanel *panel = CompilationPanelFrom(widget);
    if (!panel->changing && panel->pending == NULL)
    {
        panel->changing = true;
        CompilationShowRow(panel);
        panel->changing = false;
    }
    g_object_unref(widget);
}
static void CompilationWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    CompilationRead *read = data;
    read->status = UmiCompilationDatabaseRead(read->directory, read->cancel, &read->database);
    read->find_status = UMI_STATUS_OK;
    if (read->status == UMI_STATUS_OK && read->source[0] != '\0')
        read->find_status =
            UmiCompilationDatabaseFind(read->database, read->source, 0U, &read->selected);
    g_task_return_boolean(task, TRUE);
}
static void CompilationReadDone(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    CompilationRead *read = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    GtkWidget *widget = g_weak_ref_get(&read->panel);
    CompilationPanel *panel = widget == NULL ? NULL : CompilationPanelFrom(widget);
    if (panel != NULL && panel->pending == read)
    {
        panel->pending = NULL;
        panel->changing = true;
        g_object_set_data(G_OBJECT(widget), "umicom-compilation-pending", NULL);
        bool current = panel->generation == read->generation &&
                       !umi_cancellation_token_is_requested(read->cancel);
        if (CompilationPanelLive(panel) && current && read->status == UMI_STATUS_OK)
        {
            UmiCompilationDatabaseDestroy(panel->database);
            panel->database = read->database;
            read->database = NULL;
            memcpy(panel->reviewed_directory, read->directory, strlen(read->directory) + 1U);
            size_t count = UmiCompilationDatabaseCount(panel->database);
            panel->ready = true;
            gtk_spin_button_set_range(GTK_SPIN_BUTTON(panel->row), 1.0,
                                      count == 0U ? 1.0 : (double)count);
            if (CompilationPanelLive(panel))
                gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->row),
                                          (double)(read->selected + 1U));
            if (CompilationPanelLive(panel))
                CompilationShowRow(panel);
            if (CompilationPanelLive(panel))
            {
                char *message = g_strdup_printf(
                    "Read %zu command rows. %sNo command was executed. This snapshot does not "
                    "prove freshness.",
                    count,
                    read->source[0] == '\0'              ? ""
                    : read->find_status == UMI_STATUS_OK ? "Showing the first matching source; "
                                                           "other rows may use different settings. "
                    : read->find_status == UMI_STATUS_NOT_FOUND
                        ? "The requested source is not recorded. "
                        : "The requested source path could not be matched. ");
                gtk_label_set_text(GTK_LABEL(panel->message), message);
                g_free(message);
            }
        }
        else if (CompilationPanelLive(panel))
        {
            char *message =
                current ? g_strdup_printf("Cannot read compiler database: %s.",
                                          umi_status_text(read->status))
                        : g_strdup("The read was cancelled or its inputs changed. Read again.");
            gtk_label_set_text(GTK_LABEL(panel->message), message);
            g_free(message);
        }
        /* GTK notifications may have changed a draft while publishing widgets. */
        if (panel->generation != read->generation)
            panel->ready = false;
        if (CompilationPanelLive(panel))
            gtk_widget_set_sensitive(panel->use, panel->ready);
        if (CompilationPanelLive(panel))
            gtk_widget_set_sensitive(panel->cancel, FALSE);
        panel->changing = false;
        if (CompilationPanelLive(panel))
            gtk_widget_set_sensitive(panel->read, TRUE);
    }
    g_clear_object(&widget);
    CompilationReadFree(read);
}
static void CompilationReadClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *widget = g_object_ref(GTK_WIDGET(data));
    CompilationPanel *panel = CompilationPanelFrom(widget);
    if (!CompilationPanelLive(panel) || panel->changing || panel->pending != NULL ||
        panel->generation == UINT64_MAX)
    {
        g_object_unref(widget);
        return;
    }
    CompilationRead *read = g_try_new0(CompilationRead, 1U);
    if (read == NULL)
    {
        g_object_unref(widget);
        return;
    }
    g_weak_ref_init(&read->panel, widget);
    UmiStatus status = umi_text_copy(read->directory, sizeof read->directory,
                                     gtk_editable_get_text(GTK_EDITABLE(panel->directory)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(read->source, sizeof read->source,
                               gtk_editable_get_text(GTK_EDITABLE(panel->source)));
    if (status == UMI_STATUS_OK)
        status = umi_cancellation_token_create(&read->cancel);
    if (status != UMI_STATUS_OK)
    {
        CompilationReadFree(read);
        gtk_label_set_text(GTK_LABEL(panel->message),
                           "Input is too long or the read could not be allocated.");
        g_object_unref(widget);
        return;
    }
    read->generation = panel->generation;
    panel->pending = read;
    panel->ready = false;
    panel->changing = true;
    g_object_set_data(G_OBJECT(widget), "umicom-compilation-pending", GINT_TO_POINTER(1));
    gtk_widget_set_sensitive(panel->use, FALSE);
    if (CompilationPanelLive(panel))
        gtk_widget_set_sensitive(panel->read, FALSE);
    if (CompilationPanelLive(panel))
        gtk_widget_set_sensitive(panel->cancel, TRUE);
    if (CompilationPanelLive(panel))
        gtk_label_set_text(GTK_LABEL(panel->message), "Reading compiler commands...");
    panel->changing = false;
    /* Even an immediate close still dispatches the cancelled request so its
     * single completion path releases all copied data without touching widgets. */
    GTask *task = g_task_new(NULL, NULL, CompilationReadDone, NULL);
    g_task_set_task_data(task, read, NULL);
    g_task_run_in_thread(task, CompilationWorker);
    g_object_unref(task);
    g_object_unref(widget);
}
static void CompilationCancelClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *widget = g_object_ref(GTK_WIDGET(data));
    CompilationPanel *panel = CompilationPanelFrom(widget);
    if (CompilationPanelLive(panel) && panel->pending != NULL)
        umi_cancellation_token_request(panel->pending->cancel);
    g_object_unref(widget);
}
static void CompilationUseClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *widget = g_object_ref(GTK_WIDGET(data));
    CompilationPanel *panel = CompilationPanelFrom(widget);
    if (!CompilationPanelLive(panel) || panel->changing || !panel->ready || panel->pending != NULL)
    {
        g_object_unref(widget);
        return;
    }
    GtkEditable *arguments = g_weak_ref_get(&panel->arguments);
    if (arguments == NULL || !GTK_IS_WIDGET(arguments) ||
        gtk_widget_get_root(GTK_WIDGET(arguments)) != gtk_widget_get_root(widget))
    {
        g_clear_object(&arguments);
        g_object_unref(widget);
        return;
    }
    panel->changing = true;
    char prepared[2048];
    UmiStatus status = UmiCompilationDatabaseClangdArguments(
        gtk_editable_get_text(arguments), panel->reviewed_directory, prepared, sizeof prepared);
    if (status == UMI_STATUS_OK)
        gtk_editable_set_text(arguments, prepared);
    if (CompilationPanelLive(panel))
    {
        char *message =
            status == UMI_STATUS_OK
                ? g_strdup("Clangd arguments prepared. Review the executable and arguments, then "
                           "request the source operation. Nothing was launched or saved.")
                : g_strdup_printf("Cannot prepare clangd arguments: %s. Check duplicate options, "
                                  "response files and length.",
                                  umi_status_text(status));
        gtk_label_set_text(GTK_LABEL(panel->message), message);
        g_free(message);
    }
    panel->changing = false;
    g_object_unref(arguments);
    g_object_unref(widget);
}
static GtkWidget *CompilationControl(CompilationPanel *panel, GtkWidget *widget, const char *id)
{
    g_ptr_array_add(panel->controls, g_object_ref_sink(widget));
    (void)umi_gtk4_automation_tag_widget(widget, id);
    gtk_box_append(GTK_BOX(panel->panel), widget);
    return widget;
}
static GtkWidget *CompilationEntry(CompilationPanel *panel, const char *title, const char *id,
                                   const char *value)
{
    GtkWidget *label = gtk_label_new(title);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(panel->panel), label);
    GtkWidget *entry = CompilationControl(panel, gtk_entry_new(), id);
    gtk_editable_set_text(GTK_EDITABLE(entry), value == NULL ? "" : value);
    g_signal_connect_object(entry, "changed", G_CALLBACK(CompilationChanged), panel->panel, 0);
    return entry;
}
UmiStatus UmiGtk4CompilationDatabasePanelCreate(const char *directory, const char *source,
                                                GtkEditable *arguments, GtkWidget **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (arguments != NULL && (!GTK_IS_EDITABLE(arguments) || !GTK_IS_WIDGET(arguments)))
        return UMI_STATUS_INVALID_ARGUMENT;
    CompilationPanel *panel = g_try_new0(CompilationPanel, 1U);
    if (panel == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    panel->panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    panel->controls = g_ptr_array_new_with_free_func(g_object_unref);
    panel->generation = 1U;
    g_weak_ref_init(&panel->arguments, arguments);
    g_object_set_data_full(G_OBJECT(panel->panel), "umicom-compilation-panel", panel,
                           CompilationPanelFree);
    (void)umi_gtk4_automation_tag_widget(panel->panel, "compiler.database.panel");
    GtkWidget *help =
        gtk_label_new("Compiler commands describe include paths, definitions and language flags. "
                      "Read a trusted build folder's compile_commands.json. These records are "
                      "displayed, never executed.");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_label_set_xalign(GTK_LABEL(help), 0.0F);
    gtk_box_append(GTK_BOX(panel->panel), help);
    panel->directory = CompilationEntry(panel, "Compiler database folder (absolute)",
                                        "compiler.database.directory", directory);
    panel->source = CompilationEntry(panel, "Find source file (optional absolute path)",
                                     "compiler.database.source", source);
    panel->read = CompilationControl(panel, gtk_button_new_with_label("Read compiler database"),
                                     "compiler.database.read");
    panel->cancel = CompilationControl(panel, gtk_button_new_with_label("Cancel read"),
                                       "compiler.database.cancel");
    GtkWidget *label = gtk_label_new("Command row (multiple rows can describe the same source)");
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(panel->panel), label);
    panel->row = CompilationControl(panel, gtk_spin_button_new_with_range(1.0, 1.0, 1.0),
                                    "compiler.database.row");
    panel->details = gtk_text_view_new();
    g_ptr_array_add(panel->controls, g_object_ref_sink(panel->details));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->details), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->details), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->details), GTK_WRAP_WORD_CHAR);
    (void)umi_gtk4_automation_tag_widget(panel->details, "compiler.database.details");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 160);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->details);
    gtk_box_append(GTK_BOX(panel->panel), scroll);
    panel->use = CompilationControl(panel, gtk_button_new_with_label("Use this folder for clangd"),
                                    "compiler.database.use");
    gtk_widget_set_visible(panel->use, arguments != NULL);
    panel->message = CompilationControl(panel, gtk_label_new("No compiler database has been read."),
                                        "compiler.database.message");
    gtk_label_set_wrap(GTK_LABEL(panel->message), TRUE);
    gtk_label_set_selectable(GTK_LABEL(panel->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->message), 0.0F);
    gtk_widget_set_sensitive(panel->use, FALSE);
    gtk_widget_set_sensitive(panel->cancel, FALSE);
    g_signal_connect_object(panel->read, "clicked", G_CALLBACK(CompilationReadClicked),
                            panel->panel, 0);
    g_signal_connect_object(panel->cancel, "clicked", G_CALLBACK(CompilationCancelClicked),
                            panel->panel, 0);
    g_signal_connect_object(panel->use, "clicked", G_CALLBACK(CompilationUseClicked), panel->panel,
                            0);
    g_signal_connect_object(panel->row, "value-changed", G_CALLBACK(CompilationRowChanged),
                            panel->panel, 0);
    g_signal_connect(panel->panel, "map", G_CALLBACK(CompilationMapped), NULL);
    g_signal_connect(panel->panel, "unmap", G_CALLBACK(CompilationUnmapped), NULL);
    *out = panel->panel;
    return UMI_STATUS_OK;
}

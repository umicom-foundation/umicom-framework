/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/program_console_gtk4.c
 * PURPOSE: Connect bounded interactive process input to GTK without retaining a closed workbench.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/program_console.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>

typedef struct ConsoleWork
{
    GWeakRef panel;
    UmiProgramConsole *console;
    UmiProgramConsoleSnapshot snapshot;
} ConsoleWork;
typedef struct ConsolePanel
{
    GtkWidget *root, *run, *stop, *input, *send, *end, *output, *errors, *status;
    GPtrArray *controls;
    UmiGtk4ProgramConsoleCallbacks callbacks;
    ConsoleWork *pending;
    guint timer;
    bool changing;
} ConsolePanel;
static ConsolePanel *ConsolePanelFrom(GtkWidget *root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-program-console");
}
static bool ConsoleLive(ConsolePanel *panel)
{
    if (panel == NULL || !gtk_widget_get_mapped(panel->root))
        return false;
    GtkRoot *root = gtk_widget_get_root(panel->root);
    return GTK_IS_WINDOW(root) && UmiGtk4WindowIsOpen(GTK_WINDOW(root));
}
static bool ConsoleAuthorized(ConsolePanel *panel)
{
    /* Hosts can emit notifications while checking authority. Test liveness again
     * before a callback result is allowed to trigger another operation. */
    if (!ConsoleLive(panel))
        return false;
    return panel->callbacks.authorized(panel->callbacks.context) && ConsoleLive(panel);
}
static void ConsoleWorkFree(ConsoleWork *work)
{
    g_weak_ref_clear(&work->panel);
    (void)UmiProgramConsoleDestroy(&work->console);
    g_free(work);
}
static void ConsolePanelFree(gpointer context)
{
    ConsolePanel *panel = context;
    if (panel->timer != 0U)
        g_source_remove(panel->timer);
    if (panel->pending != NULL)
        (void)UmiProgramConsoleStop(panel->pending->console);
    if (panel->callbacks.destroy_context != NULL)
        panel->callbacks.destroy_context(panel->callbacks.context);
    g_ptr_array_unref(panel->controls);
    g_free(panel);
}
static void ConsoleStatus(ConsolePanel *panel, const char *message)
{
    if (ConsoleLive(panel))
        gtk_label_set_text(GTK_LABEL(panel->status), message);
}
static void ConsoleText(GtkWidget *view, const char *bytes)
{
    char *valid = g_utf8_make_valid(bytes, -1);
    const char *previous = g_object_get_data(G_OBJECT(view), "umicom-console-rendered");
    if (previous == NULL || strcmp(previous, valid) != 0)
    {
        /* Avoid replacing an unchanged buffer, which would discard text selection. */
        g_object_set_data_full(G_OBJECT(view), "umicom-console-rendered", g_strdup(valid), g_free);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)), valid, -1);
    }
    g_free(valid);
}
static void ConsoleShow(ConsolePanel *panel, ConsoleWork *work)
{
    UmiProgramConsoleSnapshot *snapshot = &work->snapshot;
    (void)UmiProgramConsoleRead(work->console, snapshot);
    if (!ConsoleLive(panel))
        return;
    panel->changing = true;
    ConsoleText(panel->output, snapshot->output);
    if (ConsoleLive(panel))
        ConsoleText(panel->errors, snapshot->diagnostics);
    if (ConsoleLive(panel))
        gtk_widget_set_sensitive(panel->send, snapshot->started && !snapshot->completed &&
                                                  !snapshot->input_ended &&
                                                  !snapshot->stop_requested);
    if (ConsoleLive(panel))
        gtk_widget_set_sensitive(panel->end, !snapshot->completed && !snapshot->input_ended &&
                                                 !snapshot->stop_requested);
    if (ConsoleLive(panel))
        gtk_widget_set_sensitive(panel->stop, !snapshot->completed);
    if (ConsoleLive(panel))
    {
        char *message =
            g_strdup_printf("%s | exit %d | %zu queued input frames%s%s",
                            snapshot->completed        ? umi_status_text(snapshot->status)
                            : snapshot->stop_requested ? "Stopping"
                            : snapshot->started        ? "Running"
                                                       : "Starting",
                            snapshot->exit_code, snapshot->queued_frames,
                            snapshot->output_truncated ? " | earlier output omitted" : "",
                            snapshot->diagnostics_truncated ? " | earlier errors omitted" : "");
        ConsoleStatus(panel, message);
        g_free(message);
    }
    panel->changing = false;
}
static void ConsoleWeakFree(gpointer context)
{
    GWeakRef *weak = context;
    g_weak_ref_clear(weak);
    g_free(weak);
}
static gboolean ConsoleTick(gpointer context)
{
    GtkWidget *root = g_weak_ref_get(context);
    if (root == NULL)
        return G_SOURCE_REMOVE;
    ConsolePanel *panel = ConsolePanelFrom(root);
    if (panel == NULL || panel->pending == NULL)
    {
        if (panel != NULL)
            panel->timer = 0U;
        g_object_unref(root);
        return G_SOURCE_REMOVE;
    }
    if (!ConsoleAuthorized(panel))
        (void)UmiProgramConsoleStop(panel->pending->console);
    ConsoleShow(panel, panel->pending);
    g_object_unref(root);
    return G_SOURCE_CONTINUE;
}
static void ConsoleWorker(GTask *task, gpointer source, gpointer context, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    ConsoleWork *work = context;
    (void)UmiProgramConsoleRun(work->console);
    g_task_return_boolean(task, TRUE);
}
static void ConsoleDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    (void)context;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    ConsoleWork *work = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    GtkWidget *root = g_weak_ref_get(&work->panel);
    ConsolePanel *panel = root != NULL ? ConsolePanelFrom(root) : NULL;
    if (panel != NULL && panel->pending == work)
    {
        if (panel->timer != 0U)
        {
            g_source_remove(panel->timer);
            panel->timer = 0U;
        }
        ConsoleShow(panel, work);
        panel->pending = NULL;
        g_object_set_data(G_OBJECT(root), "umicom-console-pending", NULL);
        panel->changing = true;
        if (ConsoleLive(panel))
            gtk_widget_set_sensitive(panel->run, TRUE);
        panel->changing = false;
    }
    g_clear_object(&root);
    ConsoleWorkFree(work);
}
static void ConsoleRun(GtkButton *button, gpointer context)
{
    (void)button;
    GtkWidget *root = g_object_ref(context);
    ConsolePanel *panel = ConsolePanelFrom(root);
    if (!ConsoleLive(panel) || panel->pending != NULL || panel->changing)
    {
        g_object_unref(root);
        return;
    }
    panel->changing = true;
    ConsoleWork *work = g_try_new0(ConsoleWork, 1U);
    if (work == NULL)
    {
        panel->changing = false;
        g_object_unref(root);
        return;
    }
    g_weak_ref_init(&work->panel, root);
    UmiStatus status = ConsoleAuthorized(panel)
                           ? panel->callbacks.prepare(&work->console, panel->callbacks.context)
                           : UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && work->console == NULL)
        status = UMI_STATUS_INVALID_STATE;
    if (status != UMI_STATUS_OK)
    {
        char *message =
            g_strdup_printf("Cannot start program console: %s.", umi_status_text(status));
        ConsoleStatus(panel, message);
        g_free(message);
        ConsoleWorkFree(work);
        panel->changing = false;
        g_object_unref(root);
        return;
    }
    panel->pending = work;
    g_object_set_data(G_OBJECT(root), "umicom-console-pending", GINT_TO_POINTER(1));
    if (!ConsoleAuthorized(panel))
        (void)UmiProgramConsoleStop(work->console);
    if (ConsoleLive(panel))
        gtk_widget_set_sensitive(panel->run, FALSE);
    ConsoleStatus(panel, "Starting the selected program...");
    panel->changing = false;
    GWeakRef *weak = g_new0(GWeakRef, 1U);
    g_weak_ref_init(weak, root);
    panel->timer = g_timeout_add_full(G_PRIORITY_DEFAULT, 100U, ConsoleTick, weak, ConsoleWeakFree);
    /* The task owns the console until native cleanup finishes. Closing the panel
     * merely requests Stop; no GTK object is accessed from the worker. */
    GTask *task = g_task_new(NULL, NULL, ConsoleDone, NULL);
    g_task_set_task_data(task, work, NULL);
    g_task_run_in_thread(task, ConsoleWorker);
    g_object_unref(task);
    g_object_unref(root);
}
static void ConsoleSend(GtkWidget *control, gpointer context)
{
    (void)control;
    GtkWidget *root = g_object_ref(context);
    ConsolePanel *panel = ConsolePanelFrom(root);
    if (!panel->changing && panel->pending != NULL)
    {
        panel->changing = true;
        if (!ConsoleAuthorized(panel))
        {
            (void)UmiProgramConsoleStop(panel->pending->console);
            panel->changing = false;
            g_object_unref(root);
            return;
        }
        const char *text = gtk_editable_get_text(GTK_EDITABLE(panel->input));
        size_t length = strlen(text);
        UmiStatus status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (length < UMI_PROGRAM_CONSOLE_INPUT_CAPACITY)
        {
            unsigned char frame[UMI_PROGRAM_CONSOLE_INPUT_CAPACITY];
            memcpy(frame, text, length);
            frame[length++] = '\n';
            status = UmiProgramConsoleSend(panel->pending->console, frame, length);
            memset(frame, 0, sizeof frame);
        }
        if (status == UMI_STATUS_OK && ConsoleLive(panel))
            gtk_editable_set_text(GTK_EDITABLE(panel->input), "");
        ConsoleStatus(panel,
                      status == UMI_STATUS_OK ? "Input queued."
                      : status == UMI_STATUS_BUSY
                          ? "Input queue is full. Wait for the program to read."
                          : "Input was not accepted. Check its length and the program state.");
        panel->changing = false;
    }
    g_object_unref(root);
}
static void ConsoleEnd(GtkButton *button, gpointer context)
{
    (void)button;
    GtkWidget *root = g_object_ref(context);
    ConsolePanel *panel = ConsolePanelFrom(root);
    if (!panel->changing && panel->pending != NULL)
    {
        panel->changing = true;
        if (ConsoleAuthorized(panel))
            (void)UmiProgramConsoleEndInput(panel->pending->console);
        else
            (void)UmiProgramConsoleStop(panel->pending->console);
        ConsoleShow(panel, panel->pending);
        panel->changing = false;
    }
    g_object_unref(root);
}
static void ConsoleStop(GtkButton *button, gpointer context)
{
    (void)button;
    GtkWidget *root = g_object_ref(context);
    ConsolePanel *panel = ConsolePanelFrom(root);
    if (ConsoleLive(panel) && panel->pending != NULL)
        (void)UmiProgramConsoleStop(panel->pending->console);
    g_object_unref(root);
}
static void ConsoleUnmap(GtkWidget *root, gpointer context)
{
    (void)context;
    ConsolePanel *panel = ConsolePanelFrom(root);
    if (panel->pending != NULL)
        (void)UmiProgramConsoleStop(panel->pending->console);
}
/* A hidden panel can finish while no controls are mapped. Restore idle
 * sensitivity on the next map without restarting or replaying any input. */
static void ConsoleMap(GtkWidget *root, gpointer context)
{
    (void)context;
    g_object_ref(root);
    ConsolePanel *panel = ConsolePanelFrom(root);
    panel->changing = true;
    gtk_widget_set_sensitive(panel->run, panel->pending == NULL);
    if (ConsoleLive(panel) && panel->pending != NULL)
        ConsoleShow(panel, panel->pending);
    if (ConsoleLive(panel) && panel->pending == NULL)
    {
        gtk_widget_set_sensitive(panel->send, FALSE);
        if (ConsoleLive(panel))
            gtk_widget_set_sensitive(panel->end, FALSE);
        if (ConsoleLive(panel))
            gtk_widget_set_sensitive(panel->stop, FALSE);
    }
    panel->changing = false;
    g_object_unref(root);
}
static GtkWidget *ConsoleControl(ConsolePanel *panel, GtkWidget *widget, const char *tag)
{
    g_ptr_array_add(panel->controls, g_object_ref_sink(widget));
    (void)umi_gtk4_automation_tag_widget(widget, tag);
    return widget;
}
UmiStatus UmiGtk4ProgramConsolePanelCreate(const UmiGtk4ProgramConsoleCallbacks *callbacks,
                                           GtkWidget **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (callbacks == NULL || callbacks->prepare == NULL || callbacks->authorized == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    ConsolePanel *panel = g_try_new0(ConsolePanel, 1U);
    if (panel == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    panel->callbacks = *callbacks;
    panel->controls = g_ptr_array_new_with_free_func(g_object_unref);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-program-console", panel,
                           ConsolePanelFree);
    (void)umi_gtk4_automation_tag_widget(panel->root, "program.console.panel");
    panel->run = ConsoleControl(panel, gtk_button_new_with_label("Run existing program"),
                                "program.console.run");
    panel->stop = ConsoleControl(panel, gtk_button_new_with_label("Stop"), "program.console.stop");
    panel->input = ConsoleControl(panel, gtk_entry_new(), "program.console.input");
    panel->send =
        ConsoleControl(panel, gtk_button_new_with_label("Send line"), "program.console.send");
    panel->end =
        ConsoleControl(panel, gtk_button_new_with_label("End input"), "program.console.end");
    panel->output = ConsoleControl(panel, gtk_text_view_new(), "program.console.output");
    panel->errors = ConsoleControl(panel, gtk_text_view_new(), "program.console.errors");
    panel->status = ConsoleControl(
        panel,
        gtk_label_new("Run uses the selected existing program. Build and save first when needed."),
        "program.console.status");
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_append(GTK_BOX(actions), panel->run);
    gtk_box_append(GTK_BOX(actions), panel->stop);
    gtk_box_append(GTK_BOX(panel->root), actions);
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    GtkWidget *views[] = {panel->output, panel->errors};
    GtkWidget *tabs = gtk_notebook_new();
    for (size_t index = 0U; index < 2U; ++index)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(views[index]), FALSE);
        gtk_text_view_set_monospace(GTK_TEXT_VIEW(views[index]), TRUE);
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(views[index]), GTK_WRAP_CHAR);
        GtkWidget *scroll = gtk_scrolled_window_new();
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), views[index]);
        gtk_widget_set_vexpand(scroll, TRUE);
        gtk_notebook_append_page(GTK_NOTEBOOK(tabs), scroll,
                                 gtk_label_new(index == 0U ? "Output" : "Errors"));
    }
    gtk_widget_set_size_request(tabs, -1, 220);
    gtk_widget_set_vexpand(tabs, TRUE);
    gtk_box_append(GTK_BOX(panel->root), tabs);
    GtkWidget *input_row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_set_hexpand(panel->input, TRUE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(panel->input), "Type input for the running program");
    gtk_box_append(GTK_BOX(input_row), panel->input);
    gtk_box_append(GTK_BOX(input_row), panel->send);
    gtk_box_append(GTK_BOX(input_row), panel->end);
    gtk_box_append(GTK_BOX(panel->root), input_row);
    GtkWidget *note = gtk_label_new(
        "Send line adds a newline. End input sends EOF after queued text. Closing this console "
        "stops its program. Full-screen terminal applications need an external terminal.");
    gtk_label_set_wrap(GTK_LABEL(note), TRUE);
    gtk_box_append(GTK_BOX(panel->root), note);
    gtk_widget_set_sensitive(panel->send, FALSE);
    gtk_widget_set_sensitive(panel->end, FALSE);
    gtk_widget_set_sensitive(panel->stop, FALSE);
    g_signal_connect_object(panel->run, "clicked", G_CALLBACK(ConsoleRun), panel->root, 0);
    g_signal_connect_object(panel->stop, "clicked", G_CALLBACK(ConsoleStop), panel->root, 0);
    g_signal_connect_object(panel->send, "clicked", G_CALLBACK(ConsoleSend), panel->root, 0);
    g_signal_connect_object(panel->input, "activate", G_CALLBACK(ConsoleSend), panel->root, 0);
    g_signal_connect_object(panel->end, "clicked", G_CALLBACK(ConsoleEnd), panel->root, 0);
    g_signal_connect(panel->root, "map", G_CALLBACK(ConsoleMap), NULL);
    g_signal_connect(panel->root, "unmap", G_CALLBACK(ConsoleUnmap), NULL);
    *out = panel->root;
    return UMI_STATUS_OK;
}
bool UmiGtk4ProgramConsolePanelPending(GtkWidget *root)
{
    if (!GTK_IS_WIDGET(root))
        return false;
    ConsolePanel *panel = ConsolePanelFrom(root);
    return panel != NULL && panel->pending != NULL;
}

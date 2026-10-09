/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/live_diagnostics_gtk4.c
 * PURPOSE: Keep live source capture on GTK and native protocol ownership on a cancellable worker.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/text.h"
/* The panel copies the same document URI as the Framework diagnostic session.
 * Use the editor contract's owning header so the bound stays shared across
 * frontends and remains independent of transitive GTK adapter includes. */
#include "umicom/editor/source_location.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/compilation_database.h"
#include "umicom/ui/gtk4/live_diagnostics.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <string.h>
typedef struct LiveDiagnosticWork
{
    GWeakRef panel;
    UmiLanguageDiagnosticMonitor *monitor;
} LiveDiagnosticWork;
typedef struct LiveDiagnosticPanel
{
    GtkWidget *root, *program, *arguments, *start, *stop, *previous, *next, *output, *status,
        *database, *selected, *open_source;
    GPtrArray *controls;
    UmiGtk4LiveDiagnosticNavigate navigate;
    char *display_source;
    size_t display_bytes;
    UmiGtk4LiveDiagnosticCallbacks callbacks;
    UmiLanguageDiagnosticMonitorConfig config;
    char directory[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY], tools[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY];
    char root_uri[UMI_LANGUAGE_RUNTIME_PATH_CAPACITY], uri[UMI_EDITOR_SOURCE_URI_CAPACITY],
        language[128];
    LiveDiagnosticWork *pending;
    UmiLanguageDiagnosticCatalogue *catalogue;
    uint64_t sequence;
    size_t page;
    guint timer;
    bool changing;
} LiveDiagnosticPanel;
static LiveDiagnosticPanel *LiveDiagnosticFrom(GtkWidget *root)
{
    return g_object_get_data(G_OBJECT(root), "umicom-live-diagnostics");
}
static bool LiveDiagnosticVisible(LiveDiagnosticPanel *panel)
{
    if (panel == NULL || !gtk_widget_get_mapped(panel->root))
        return false;
    GtkRoot *root = gtk_widget_get_root(panel->root);
    return GTK_IS_WINDOW(root) && UmiGtk4WindowIsOpen(GTK_WINDOW(root));
}
static bool LiveDiagnosticAuthorized(LiveDiagnosticPanel *panel)
{
    return LiveDiagnosticVisible(panel) && panel->callbacks.authorized(panel->callbacks.context) &&
           LiveDiagnosticVisible(panel);
}
static void LiveDiagnosticStatus(LiveDiagnosticPanel *panel, const char *message)
{
    if (LiveDiagnosticVisible(panel))
        gtk_label_set_text(GTK_LABEL(panel->status), message);
}
static void LiveDiagnosticRetire(LiveDiagnosticPanel *panel)
{
    UmiLanguageDiagnosticCatalogueDestroy(panel->catalogue);
    panel->catalogue = NULL;
    g_clear_pointer(&panel->display_source, g_free);
    panel->display_bytes = 0U;
    panel->page = 0U;
    /* Controls have independent strong references. Clear hidden output too so a
     * remapped panel cannot redisplay diagnostics from a retired session. */
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->output)), "", 0);
    gtk_widget_set_sensitive(panel->previous, FALSE);
    gtk_widget_set_sensitive(panel->next, FALSE);
    gtk_widget_set_sensitive(panel->selected, FALSE);
    gtk_widget_set_sensitive(panel->open_source, FALSE);
}
static void LiveDiagnosticRender(LiveDiagnosticPanel *panel)
{
    size_t count = UmiLanguageDiagnosticCatalogueCount(panel->catalogue);
    size_t first = panel->page * 32U, end = first + 32U < count ? first + 32U : count;
    GString *text = g_string_new(NULL);
    g_string_append_printf(text, "Diagnostics %zu–%zu of %zu\n\n", count == 0U ? 0U : first + 1U,
                           end, count);
    for (size_t index = first; index < end; ++index)
    {
        UmiLanguageDiagnostic diagnostic;
        if (UmiLanguageDiagnosticCatalogueAt(panel->catalogue, index, &diagnostic) != UMI_STATUS_OK)
            break;
        const char *severity = diagnostic.severity == 1   ? "Error"
                               : diagnostic.severity == 2 ? "Warning"
                               : diagnostic.severity == 3 ? "Information"
                                                          : "Hint";
        g_string_append_printf(text, "%zu. %s — line %llu, UTF-16 column %llu\n", index + 1U,
                               severity, (unsigned long long)diagnostic.range.start.line + 1U,
                               (unsigned long long)diagnostic.range.start.utf16_column + 1U);
        size_t bytes = strlen(diagnostic.message), shown = bytes > 8192U ? 8192U : bytes;
        while (shown != 0U && ((unsigned char)diagnostic.message[shown] & 0xc0U) == 0x80U)
            --shown;
        g_string_append_len(text, diagnostic.message, (gssize)shown);
        if (shown < bytes)
            g_string_append(text, "\n[Message shortened in this view]");
        g_string_append(text, "\n\n");
    }
    if (LiveDiagnosticVisible(panel))
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->output)), text->str,
                                 (gint)text->len);
    if (LiveDiagnosticVisible(panel))
        gtk_widget_set_sensitive(panel->previous, panel->page != 0U);
    if (LiveDiagnosticVisible(panel))
        gtk_widget_set_sensitive(panel->next, end < count);
    if (LiveDiagnosticVisible(panel))
    {
        gtk_spin_button_set_range(GTK_SPIN_BUTTON(panel->selected), 1.0,
                                  count == 0U ? 1.0 : (double)count);
        gtk_widget_set_sensitive(panel->selected, count != 0U && panel->navigate != NULL);
        gtk_widget_set_sensitive(panel->open_source, count != 0U && panel->navigate != NULL);
    }
    g_string_free(text, TRUE);
}
static void LiveDiagnosticWorkFree(LiveDiagnosticWork *work)
{
    g_weak_ref_clear(&work->panel);
    (void)UmiLanguageDiagnosticMonitorDestroy(&work->monitor);
    g_free(work);
}
static void LiveDiagnosticFree(gpointer data)
{
    LiveDiagnosticPanel *panel = data;
    if (panel->timer != 0U)
        g_source_remove(panel->timer);
    if (panel->pending != NULL)
        (void)UmiLanguageDiagnosticMonitorStop(panel->pending->monitor);
    UmiLanguageDiagnosticCatalogueDestroy(panel->catalogue);
    g_free(panel->display_source);
    if (panel->callbacks.destroy_context != NULL)
        panel->callbacks.destroy_context(panel->callbacks.context);
    g_ptr_array_unref(panel->controls);
    g_free(panel);
}
static void LiveDiagnosticWeakFree(gpointer data)
{
    GWeakRef *weak = data;
    g_weak_ref_clear(weak);
    g_free(weak);
}
static UmiStatus LiveDiagnosticCapture(LiveDiagnosticPanel *panel, char **out, size_t *bytes)
{
    *out = NULL;
    *bytes = 0U;
    if (!LiveDiagnosticAuthorized(panel))
        return UMI_STATUS_PERMISSION_DENIED;
    char *text = g_try_malloc(UMI_LANGUAGE_RUNTIME_JSON_CAPACITY);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = panel->callbacks.capture(text, UMI_LANGUAGE_RUNTIME_JSON_CAPACITY, bytes,
                                                panel->callbacks.context);
    if (status == UMI_STATUS_OK && *bytes >= UMI_LANGUAGE_RUNTIME_JSON_CAPACITY)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK && !LiveDiagnosticAuthorized(panel))
        status = UMI_STATUS_PERMISSION_DENIED;
    if (status != UMI_STATUS_OK)
    {
        g_free(text);
        return status;
    }
    text[*bytes] = '\0';
    *out = text;
    return UMI_STATUS_OK;
}
static void LiveDiagnosticSensitivity(LiveDiagnosticPanel *panel)
{
    GtkWidget *fields[] = {panel->program, panel->arguments, panel->start};
    for (size_t index = 0U;
         index < sizeof fields / sizeof fields[0] && LiveDiagnosticVisible(panel); ++index)
        gtk_widget_set_sensitive(fields[index], panel->pending == NULL);
    if (LiveDiagnosticVisible(panel) && panel->database != NULL)
        gtk_widget_set_sensitive(panel->database, panel->pending == NULL);
    if (LiveDiagnosticVisible(panel))
        gtk_widget_set_sensitive(panel->stop, panel->pending != NULL);
}
static void LiveDiagnosticObserve(LiveDiagnosticPanel *panel)
{
    char *source = NULL;
    size_t bytes = 0U;
    UmiStatus status = LiveDiagnosticCapture(panel, &source, &bytes);
    uint64_t sequence = panel->sequence;
    if (status == UMI_STATUS_OK)
        status =
            UmiLanguageDiagnosticMonitorSubmit(panel->pending->monitor, source, bytes, &sequence);
    if (status != UMI_STATUS_OK)
    {
        g_free(source);
        (void)UmiLanguageDiagnosticMonitorStop(panel->pending->monitor);
        LiveDiagnosticRetire(panel);
        char *message = g_strdup_printf(
            "Live diagnostics stopped: %s. Reopen for the current document and workspace.",
            umi_status_text(status));
        LiveDiagnosticStatus(panel, message);
        g_free(message);
        return;
    }
    if (sequence != panel->sequence)
    {
        panel->sequence = sequence;
        LiveDiagnosticRetire(panel);
    }
    UmiLanguageDiagnosticCatalogue *catalogue = NULL;
    uint64_t publication_sequence = 0U;
    status = UmiLanguageDiagnosticMonitorTake(panel->pending->monitor, &publication_sequence,
                                              &catalogue);
    if (status == UMI_STATUS_OK && publication_sequence == panel->sequence &&
        LiveDiagnosticVisible(panel))
    {
        UmiLanguageDiagnosticCatalogueDestroy(panel->catalogue);
        panel->catalogue = catalogue;
        catalogue = NULL;
        /* Retain the exact source whose accepted sequence matches this report.
         * Navigation can compare it again even before the next edit poll. */
        g_free(panel->display_source);
        panel->display_source = source;
        panel->display_bytes = bytes;
        source = NULL;
        panel->page = 0U;
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(panel->selected), 1.0);
        LiveDiagnosticRender(panel);
    }
    g_free(source);
    UmiLanguageDiagnosticCatalogueDestroy(catalogue);
    UmiLanguageDiagnosticMonitorSnapshot snapshot;
    (void)UmiLanguageDiagnosticMonitorRead(panel->pending->monitor, &snapshot);
    if (snapshot.completed)
        LiveDiagnosticRetire(panel);
    char *message = g_strdup_printf(
        "%s | source %llu sent %llu | %llu reports ignored%s",
        snapshot.completed ? umi_status_text(snapshot.status)
        : snapshot.ready   ? "Analysing"
                           : "Starting",
        (unsigned long long)snapshot.accepted_sequence, (unsigned long long)snapshot.sent_sequence,
        (unsigned long long)snapshot.session.ignored_publications,
        snapshot.session.unversioned_publications != 0U
            ? " | unversioned reports received; changed drafts require a matching version"
            : "");
    LiveDiagnosticStatus(panel, message);
    g_free(message);
}
static gboolean LiveDiagnosticTick(gpointer data)
{
    GtkWidget *root = g_weak_ref_get(data);
    if (root == NULL)
        return G_SOURCE_REMOVE;
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    if (panel == NULL || panel->pending == NULL)
    {
        if (panel != NULL)
            panel->timer = 0U;
        g_object_unref(root);
        return G_SOURCE_REMOVE;
    }
    if (!panel->changing)
    {
        panel->changing = true;
        LiveDiagnosticObserve(panel);
        panel->changing = false;
    }
    g_object_unref(root);
    return G_SOURCE_CONTINUE;
}
static void LiveDiagnosticWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    LiveDiagnosticWork *work = data;
    (void)UmiLanguageDiagnosticMonitorRun(work->monitor);
    g_task_return_boolean(task, TRUE);
}
static void LiveDiagnosticDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    (void)context;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    LiveDiagnosticWork *work = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    GtkWidget *root = g_weak_ref_get(&work->panel);
    LiveDiagnosticPanel *panel = root != NULL ? LiveDiagnosticFrom(root) : NULL;
    if (panel != NULL && panel->pending == work)
    {
        panel->changing = true;
        if (panel->timer != 0U)
        {
            g_source_remove(panel->timer);
            panel->timer = 0U;
        }
        UmiLanguageDiagnosticMonitorSnapshot snapshot;
        (void)UmiLanguageDiagnosticMonitorRead(work->monitor, &snapshot);
        panel->pending = NULL;
        LiveDiagnosticRetire(panel);
        LiveDiagnosticSensitivity(panel);
        char *message = g_strdup_printf("Stopped: %s. Protocol close: %s; shutdown: %s.",
                                        umi_status_text(snapshot.status),
                                        umi_status_text(snapshot.session.close_status),
                                        umi_status_text(snapshot.session.shutdown_status));
        LiveDiagnosticStatus(panel, message);
        g_free(message);
        panel->changing = false;
    }
    g_clear_object(&root);
    LiveDiagnosticWorkFree(work);
}
static void LiveDiagnosticStart(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    if (!LiveDiagnosticVisible(panel) || panel->changing || panel->pending != NULL)
    {
        g_object_unref(root);
        return;
    }
    panel->changing = true;
    LiveDiagnosticRetire(panel);
    LiveDiagnosticWork *work = g_try_new0(LiveDiagnosticWork, 1U);
    char *source = NULL;
    size_t bytes = 0U;
    UmiStatus status =
        work == NULL ? UMI_STATUS_OUT_OF_MEMORY : LiveDiagnosticCapture(panel, &source, &bytes);
    if (work != NULL)
        g_weak_ref_init(&work->panel, root);
    UmiLanguageDiagnosticMonitorConfig config = panel->config;
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(config.profile.executable, sizeof config.profile.executable,
                               gtk_editable_get_text(GTK_EDITABLE(panel->program)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(config.profile.arguments, sizeof config.profile.arguments,
                               gtk_editable_get_text(GTK_EDITABLE(panel->arguments)));
    config.document.source = source;
    config.document.source_bytes = bytes;
    if (status == UMI_STATUS_OK)
        status = UmiLanguageDiagnosticMonitorCreate(&config, &work->monitor);
    g_free(source);
    if (status != UMI_STATUS_OK)
    {
        char *message =
            g_strdup_printf("Cannot start live diagnostics: %s.", umi_status_text(status));
        LiveDiagnosticStatus(panel, message);
        g_free(message);
        if (work != NULL)
            LiveDiagnosticWorkFree(work);
        panel->changing = false;
        g_object_unref(root);
        return;
    }
    panel->sequence = 1U;
    panel->pending = work;
    if (!LiveDiagnosticAuthorized(panel))
        (void)UmiLanguageDiagnosticMonitorStop(work->monitor);
    LiveDiagnosticSensitivity(panel);
    LiveDiagnosticStatus(panel, "Starting the selected language server...");
    GWeakRef *weak = g_new0(GWeakRef, 1U);
    g_weak_ref_init(weak, root);
    panel->timer = g_timeout_add_full(G_PRIORITY_DEFAULT, 350U, LiveDiagnosticTick, weak,
                                      LiveDiagnosticWeakFree);
    GTask *task = g_task_new(NULL, NULL, LiveDiagnosticDone, NULL);
    g_task_set_task_data(task, work, NULL);
    g_task_run_in_thread(task, LiveDiagnosticWorker);
    g_object_unref(task);
    panel->changing = false;
    g_object_unref(root);
}
static void LiveDiagnosticStop(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    if (LiveDiagnosticVisible(panel) && !panel->changing && panel->pending != NULL)
    {
        panel->changing = true;
        (void)UmiLanguageDiagnosticMonitorStop(panel->pending->monitor);
        LiveDiagnosticRetire(panel);
        LiveDiagnosticStatus(panel, "Stopping...");
        panel->changing = false;
    }
    g_object_unref(root);
}
static void LiveDiagnosticPage(GtkButton *button, gpointer data)
{
    GtkWidget *root = g_object_ref(data);
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    if (LiveDiagnosticVisible(panel) && !panel->changing && panel->catalogue != NULL)
    {
        panel->changing = true;
        if (GTK_WIDGET(button) == panel->previous)
        {
            if (panel->page != 0U)
                --panel->page;
        }
        else if ((panel->page + 1U) * 32U < UmiLanguageDiagnosticCatalogueCount(panel->catalogue))
            ++panel->page;
        LiveDiagnosticRender(panel);
        panel->changing = false;
    }
    g_object_unref(root);
}
/* Copy source and coordinates before calling the host: a callback may close
 * this panel and retire its catalogue. The strong root keeps the context alive. */
static void LiveDiagnosticOpenSource(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *root = g_object_ref(data);
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    if (!LiveDiagnosticVisible(panel) || panel->changing || panel->navigate == NULL ||
        panel->catalogue == NULL || panel->display_source == NULL)
    {
        g_object_unref(root);
        return;
    }
    panel->changing = true;
    int selected = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(panel->selected));
    UmiLanguageDiagnostic row;
    UmiStatus status = selected < 1 ? UMI_STATUS_INVALID_ARGUMENT
                                    : UmiLanguageDiagnosticCatalogueAt(panel->catalogue,
                                                                       (size_t)selected - 1U, &row);
    char *expected = NULL, *current = NULL;
    size_t bytes = panel->display_bytes, current_bytes = 0U;
    if (status == UMI_STATUS_OK)
    {
        expected = g_try_malloc(bytes + 1U);
        if (expected == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
        else
            memcpy(expected, panel->display_source, bytes + 1U);
    }
    if (status == UMI_STATUS_OK)
        status = LiveDiagnosticCapture(panel, &current, &current_bytes);
    if (status == UMI_STATUS_OK &&
        (bytes != current_bytes || memcmp(expected, current, bytes) != 0))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = panel->navigate(panel->uri, expected, bytes, row.range.start, row.range.end,
                                 panel->callbacks.context);
    g_free(expected);
    g_free(current);
    if (status != UMI_STATUS_OK)
    {
        LiveDiagnosticRetire(panel);
        char *message = g_strdup_printf("Source navigation was not applied: %s. Wait for a current "
                                        "report or restart diagnostics.",
                                        umi_status_text(status));
        LiveDiagnosticStatus(panel, message);
        g_free(message);
    }
    panel->changing = false;
    g_object_unref(root);
}
static void LiveDiagnosticUnmap(GtkWidget *root, gpointer data)
{
    (void)data;
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    if (panel->pending != NULL)
        (void)UmiLanguageDiagnosticMonitorStop(panel->pending->monitor);
    LiveDiagnosticRetire(panel);
}
static void LiveDiagnosticMap(GtkWidget *root, gpointer data)
{
    (void)data;
    g_object_ref(root);
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    panel->changing = true;
    LiveDiagnosticSensitivity(panel);
    panel->changing = false;
    g_object_unref(root);
}
static GtkWidget *LiveDiagnosticControl(LiveDiagnosticPanel *panel, GtkWidget *widget,
                                        const char *tag)
{
    g_ptr_array_add(panel->controls, g_object_ref_sink(widget));
    (void)umi_gtk4_automation_tag_widget(widget, tag);
    return widget;
}
UmiStatus UmiGtk4LiveDiagnosticPanelCreate(const UmiLanguageDiagnosticMonitorConfig *config,
                                           const UmiGtk4LiveDiagnosticCallbacks *callbacks,
                                           GtkWidget **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (config == NULL || callbacks == NULL || callbacks->capture == NULL ||
        callbacks->authorized == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageDiagnosticMonitor *validation = NULL;
    UmiStatus status = UmiLanguageDiagnosticMonitorCreate(config, &validation);
    if (status != UMI_STATUS_OK)
        return status;
    (void)UmiLanguageDiagnosticMonitorDestroy(&validation);
    LiveDiagnosticPanel *panel = g_try_new0(LiveDiagnosticPanel, 1U);
    if (panel == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    panel->callbacks = *callbacks;
    panel->config = *config;
    strcpy(panel->directory, config->working_directory);
    strcpy(panel->tools, config->tool_directory == NULL ? "" : config->tool_directory);
    strcpy(panel->root_uri, config->document.root_uri);
    strcpy(panel->uri, config->document.document_uri);
    strcpy(panel->language, config->document.language_id);
    panel->config.working_directory = panel->directory;
    panel->config.tool_directory = panel->tools;
    panel->config.document.root_uri = panel->root_uri;
    panel->config.document.document_uri = panel->uri;
    panel->config.document.language_id = panel->language;
    panel->config.document.source = NULL;
    panel->config.document.source_bytes = 0U;
    panel->controls = g_ptr_array_new_with_free_func(g_object_unref);
    panel->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_set_data_full(G_OBJECT(panel->root), "umicom-live-diagnostics", panel,
                           LiveDiagnosticFree);
    (void)umi_gtk4_automation_tag_widget(panel->root, "live.diagnostics.panel");
    panel->program = LiveDiagnosticControl(panel, gtk_entry_new(), "live.diagnostics.program");
    panel->arguments = LiveDiagnosticControl(panel, gtk_entry_new(), "live.diagnostics.arguments");
    gtk_editable_set_text(GTK_EDITABLE(panel->program), config->profile.executable);
    gtk_editable_set_text(GTK_EDITABLE(panel->arguments), config->profile.arguments);
    gtk_entry_set_placeholder_text(GTK_ENTRY(panel->program),
                                   "Selected language-server executable");
    gtk_entry_set_placeholder_text(GTK_ENTRY(panel->arguments), "Literal server arguments");
    gtk_box_append(GTK_BOX(panel->root), gtk_label_new("Language server"));
    gtk_box_append(GTK_BOX(panel->root), panel->program);
    gtk_box_append(GTK_BOX(panel->root), gtk_label_new("Arguments"));
    gtk_box_append(GTK_BOX(panel->root), panel->arguments);
    if (strcmp(config->document.language_id, "c") == 0 ||
        strcmp(config->document.language_id, "cpp") == 0)
    {
        char *source_file = g_filename_from_uri(config->document.document_uri, NULL, NULL);
        if (UmiGtk4CompilationDatabasePanelCreate(NULL, source_file, GTK_EDITABLE(panel->arguments),
                                                  &panel->database) == UMI_STATUS_OK)
        {
            g_ptr_array_add(panel->controls, g_object_ref_sink(panel->database));
            gtk_box_append(
                GTK_BOX(panel->root),
                gtk_label_new(
                    "Use compiler database arguments only when the selected server is clangd."));
            gtk_box_append(GTK_BOX(panel->root), panel->database);
        }
        g_free(source_file);
    }
    GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    panel->start = LiveDiagnosticControl(panel, gtk_button_new_with_label("Start live diagnostics"),
                                         "live.diagnostics.start");
    panel->stop =
        LiveDiagnosticControl(panel, gtk_button_new_with_label("Stop"), "live.diagnostics.stop");
    panel->previous = LiveDiagnosticControl(
        panel, gtk_button_new_with_label("Previous diagnostics"), "live.diagnostics.previous");
    panel->next = LiveDiagnosticControl(panel, gtk_button_new_with_label("Next diagnostics"),
                                        "live.diagnostics.next");
    GtkWidget *buttons[] = {panel->start, panel->stop, panel->previous, panel->next};
    for (size_t index = 0U; index < sizeof buttons / sizeof buttons[0]; ++index)
        gtk_box_append(GTK_BOX(actions), buttons[index]);
    gtk_box_append(GTK_BOX(panel->root), actions);
    GtkWidget *navigation = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    panel->selected = LiveDiagnosticControl(panel, gtk_spin_button_new_with_range(1.0, 1.0, 1.0),
                                            "live.diagnostics.index");
    panel->open_source = LiveDiagnosticControl(panel, gtk_button_new_with_label("Open source"),
                                               "live.diagnostics.open");
    gtk_box_append(GTK_BOX(navigation), gtk_label_new("Diagnostic number"));
    gtk_box_append(GTK_BOX(navigation), panel->selected);
    gtk_box_append(GTK_BOX(navigation), panel->open_source);
    gtk_box_append(GTK_BOX(panel->root), navigation);
    gtk_widget_set_sensitive(panel->selected, FALSE);
    gtk_widget_set_sensitive(panel->open_source, FALSE);
    g_signal_connect_object(panel->open_source, "clicked", G_CALLBACK(LiveDiagnosticOpenSource),
                            panel->root, 0);
    panel->status =
        LiveDiagnosticControl(panel,
                              gtk_label_new("Select a trusted server, then Start. Source is sent "
                                            "to that local program; no files are saved."),
                              "live.diagnostics.status");
    gtk_label_set_wrap(GTK_LABEL(panel->status), TRUE);
    gtk_label_set_xalign(GTK_LABEL(panel->status), 0.0F);
    gtk_box_append(GTK_BOX(panel->root), panel->status);
    panel->output = LiveDiagnosticControl(panel, gtk_text_view_new(), "live.diagnostics.output");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->output), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->output), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(panel->output), GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->output);
    gtk_widget_set_vexpand(scroll, TRUE);
    gtk_widget_set_size_request(scroll, -1, 240);
    gtk_box_append(GTK_BOX(panel->root), scroll);
    gtk_widget_set_sensitive(panel->stop, FALSE);
    gtk_widget_set_sensitive(panel->previous, FALSE);
    gtk_widget_set_sensitive(panel->next, FALSE);
    g_signal_connect_object(panel->start, "clicked", G_CALLBACK(LiveDiagnosticStart), panel->root,
                            0);
    g_signal_connect_object(panel->stop, "clicked", G_CALLBACK(LiveDiagnosticStop), panel->root, 0);
    g_signal_connect_object(panel->previous, "clicked", G_CALLBACK(LiveDiagnosticPage), panel->root,
                            0);
    g_signal_connect_object(panel->next, "clicked", G_CALLBACK(LiveDiagnosticPage), panel->root, 0);
    g_signal_connect(panel->root, "map", G_CALLBACK(LiveDiagnosticMap), NULL);
    g_signal_connect(panel->root, "unmap", G_CALLBACK(LiveDiagnosticUnmap), NULL);
    *out = panel->root;
    return UMI_STATUS_OK;
}
bool UmiGtk4LiveDiagnosticPanelPending(GtkWidget *root)
{
    if (!GTK_IS_WIDGET(root))
        return false;
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    return panel != NULL && panel->pending != NULL;
}

UmiStatus UmiGtk4LiveDiagnosticPanelSetNavigator(GtkWidget *root,
                                                 UmiGtk4LiveDiagnosticNavigate navigate)
{
    if (!GTK_IS_WIDGET(root))
        return UMI_STATUS_INVALID_ARGUMENT;
    LiveDiagnosticPanel *panel = LiveDiagnosticFrom(root);
    if (panel == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (panel->changing || panel->pending != NULL)
        return UMI_STATUS_BUSY;
    panel->navigate = navigate;
    /* A stopped panel has no authoritative diagnostic selection. Binding does
     * not restore an earlier report or invoke the callback. */
    gtk_widget_set_sensitive(panel->selected, FALSE);
    gtk_widget_set_sensitive(panel->open_source, FALSE);
    return UMI_STATUS_OK;
}

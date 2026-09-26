/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desktop_system_gtk4.c
 *
 * Author: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/desktop_system/gtk4.h"
#include "umicom/desktop_system/monitor.h"
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include "desktop_system_brand.inc"

typedef struct SystemView {
    GtkWidget *refresh;
    GtkWidget *storage;
    GtkWidget *summary;
    GtkTextBuffer *overview, *processes, *network, *boot;
    int busy, hasCpu;
    UmiDesktopSystemCpu previousCpu;
} SystemView;
typedef struct CaptureInput { char *storagePath; } CaptureInput;
typedef struct Completion { GWeakRef panel; } Completion;
typedef struct CaptureOutput { UmiStatus status; UmiDesktopSystemSnapshot snapshot; } CaptureOutput;
static const char *Status(UmiStatus status)
{
    switch (status) {
    case UMI_STATUS_OK: return "Available";
    case UMI_STATUS_NOT_FOUND: return "Not present on this host";
    case UMI_STATUS_PERMISSION_DENIED: return "Access denied";
    case UMI_STATUS_CAPACITY_EXCEEDED: return "Bound reached: some data omitted";
    case UMI_STATUS_NOT_IMPLEMENTED: return "Not supported by this host adapter";
    case UMI_STATUS_PARSE_ERROR: return "Source data was not valid";
    case UMI_STATUS_INVALID_ARGUMENT: return "The path or source format was not accepted";
    default: return "Unavailable: the source could not be read";
    }
}
static GtkWidget *Brand(const unsigned char *bytes, size_t size, int height)
{
    GBytes *data = g_bytes_new_static(bytes, size); GError *error = NULL;
    GdkTexture *texture = gdk_texture_new_from_bytes(data, &error); g_bytes_unref(data);
    if (!texture) { g_clear_error(&error); return gtk_label_new("Umicom"); }
    GtkWidget *picture = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture)); g_object_unref(texture);
    gtk_widget_set_size_request(picture, -1, height); gtk_widget_set_halign(picture, GTK_ALIGN_START);
    gtk_picture_set_can_shrink(GTK_PICTURE(picture), TRUE); return picture;
}
static GtkWidget *Page(GtkTextBuffer **outBuffer)
{
    GtkWidget *view = gtk_text_view_new(); gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE); gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 12); gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 12);
    *outBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    gtk_text_buffer_set_text(*outBuffer, "Choose Refresh to read this host. No data has been collected yet.", -1);
    GtkWidget *scroll = gtk_scrolled_window_new(); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_widget_set_vexpand(scroll, TRUE); return scroll;
}
static void Show(SystemView *view, const UmiDesktopSystemSnapshot *s)
{
    GString *text = g_string_new(NULL); uint32_t usage = 0;
    g_string_append_printf(text, "Source: %s%s\nCapture duration: %" PRIu64 " ms\n\nMemory: %s\n",
        s->fixture ? "FIXTURE — " : "", s->source, s->elapsedMilliseconds, Status(s->memoryStatus));
    if (s->memoryStatus == UMI_STATUS_OK)
        g_string_append_printf(text, "  Total: %" PRIu64 " MiB\n  Available: %" PRIu64 " MiB\n",
            s->memory.totalBytes / UINT64_C(1048576), s->memory.availableBytes / UINT64_C(1048576));
    g_string_append_printf(text, "\nCPU: %s\n", Status(s->cpuStatus));
    if (s->cpuStatus == UMI_STATUS_OK) {
        if (view->hasCpu && UmiDesktopSystemCpuUsage(&view->previousCpu, &s->cpu, &usage) == UMI_STATUS_OK)
            g_string_append_printf(text, "  Busy between captures: %u.%02u%%\n", usage / 100, usage % 100);
        else g_string_append(text, "  Baseline captured. Refresh again for an interval percentage.\n");
        view->previousCpu = s->cpu; view->hasCpu = 1;
    } else view->hasCpu = 0;
    g_string_append_printf(text, "\nSelected filesystem: %s\n  %s\n", s->storage.path, Status(s->storageStatus));
    if (s->storageStatus == UMI_STATUS_OK)
        g_string_append_printf(text, "  Total: %" PRIu64 " MiB\n  Available to this user: %" PRIu64 " MiB\n",
            s->storage.totalBytes / UINT64_C(1048576), s->storage.availableBytes / UINT64_C(1048576));
    g_string_append(text, "\nThese are read-only host observations, collected over an interval.\n"
        "Inside WSL, a container or a virtual machine, they describe that environment, not necessarily the physical computer.\n"
        "No setting, process, disk or network connection is changed.\n");
    gtk_text_buffer_set_text(view->overview, text->str, -1); g_string_truncate(text, 0);
    g_string_append_printf(text, "%s\nObserved: %zu | Listed: %zu | Unreadable or exited: %zu\n\n",
        Status(s->processStatus), s->processesSeen, s->processCount, s->processesUnreadable);
    for (size_t i = 0; i < s->processCount; ++i) {
        const UmiDesktopSystemProcess *p = &s->processes[i];
        g_string_append_printf(text, "PID %" PRIu64 "  parent %" PRIu64 "  [%c] %s\n", p->pid, p->parentPid, p->state, p->name);
        if (p->startKnown) g_string_append_printf(text, "  Start identity: %" PRIu64 "\n", p->startTicks);
        else g_string_append(text, "  Start identity: unavailable\n");
        if (p->residentKnown) g_string_append_printf(text, "  Resident estimate: %" PRIu64 " KiB\n", p->residentBytes / UINT64_C(1024));
        else g_string_append(text, "  Resident estimate: not collected by this adapter\n");
    }
    g_string_append(text, "\nA PID alone is not a lasting identity. No terminate/kill action is exposed here.\n");
    gtk_text_buffer_set_text(view->processes, text->str, -1); g_string_truncate(text, 0);
    g_string_append_printf(text, "%s\nObserved interfaces: %zu | Listed: %zu\n\n",
        Status(s->networkStatus), s->interfacesSeen, s->interfaceCount);
    for (size_t i = 0; i < s->interfaceCount; ++i)
        g_string_append_printf(text, "%s\n  Received: %" PRIu64 " bytes\n  Sent: %" PRIu64 " bytes\n\n",
            s->interfaces[i].name, s->interfaces[i].receivedBytes, s->interfaces[i].transmittedBytes);
    g_string_append(text, "Counters are cumulative for each interface lifetime, not current transfer rates.\n"
        "They do not prove Internet connectivity. No IP addresses, packet contents or credentials are included.\n");
    gtk_text_buffer_set_text(view->network, text->str, -1); g_string_truncate(text, 0);
    g_string_append_printf(text, "Umicom boot report: %s\n\n", Status(s->bootStatus));
    if (s->bootStatus == UMI_STATUS_OK)
        g_string_append_printf(text, "Initial boot state: %s\nCompleted checks: %u / %u\nReason: %s\nSource: %s\n\n",
            UmiBootReportStateText(s->boot.state), s->boot.completed, s->boot.planned, s->boot.reason, s->boot.sourceId);
    g_string_append(text, "An ordinary Windows or Linux installation normally has no Umicom boot report.\n"
        "A ready report describes the initial boot checks only; it does not certify the desktop or later applications.\n"
        "This panel does not restart services, install updates, mount storage or reboot the computer.\n");
    gtk_text_buffer_set_text(view->boot, text->str, -1); g_string_free(text, TRUE);
    gtk_label_set_text(GTK_LABEL(view->summary), "Capture complete. Check each tab's source status; unavailable does not mean zero.");
}
static void InputFree(gpointer data)
{ CaptureInput *input = data; g_free(input->storagePath); g_free(input); }
static void Worker(GTask *task, gpointer source, gpointer taskData, GCancellable *cancellable)
{
    (void)source; (void)cancellable; CaptureInput *input = taskData;
    CaptureOutput *output = g_new0(CaptureOutput, 1);
    UmiDesktopSystemOptions options = {input->storagePath, NULL, NULL};
    output->status = UmiDesktopSystemCapture(&options, &output->snapshot);
    /* Only plain data crosses the worker boundary. In particular, the task
     * source is NULL: no GTK finalizer can run when the worker releases it. */
    g_task_return_pointer(task, output, g_free);
}
static void Completed(GObject *source, GAsyncResult *result, gpointer userData)
{
    (void)source; Completion *completion = userData; GError *error = NULL;
    CaptureOutput *output = g_task_propagate_pointer(G_TASK(result), &error);
    GtkWidget *panel = g_weak_ref_get(&completion->panel);
    if (panel) {
        SystemView *view = g_object_get_data(G_OBJECT(panel), "umicom-system-view");
        if (view) {
            if (output && output->status == UMI_STATUS_OK) Show(view, &output->snapshot);
            else gtk_label_set_text(GTK_LABEL(view->summary), "Refresh failed. Any previous display below is an older snapshot; check the selected path.");
            view->busy = 0; gtk_widget_set_sensitive(view->refresh, TRUE);
        }
        g_object_unref(panel);
    }
    g_free(output); g_clear_error(&error); g_weak_ref_clear(&completion->panel); g_free(completion);
}
UmiStatus UmiDesktopSystemGtk4Refresh(GtkWidget *panel)
{
    if (!panel) return UMI_STATUS_INVALID_ARGUMENT;
    SystemView *view = g_object_get_data(G_OBJECT(panel), "umicom-system-view");
    if (!view) return UMI_STATUS_INVALID_ARGUMENT;
    if (view->busy) return UMI_STATUS_BUSY;
    CaptureInput *input = g_new0(CaptureInput, 1);
    const char *path = gtk_editable_get_text(GTK_EDITABLE(view->storage));
    input->storagePath = path && *path ? g_strdup(path) : NULL;
    Completion *completion = g_new0(Completion, 1); g_weak_ref_init(&completion->panel, panel);
    view->busy = 1; gtk_widget_set_sensitive(view->refresh, FALSE);
    gtk_label_set_text(GTK_LABEL(view->summary), "Refreshing. Any data still visible below belongs to the previous capture.");
    GTask *task = g_task_new(NULL, NULL, Completed, completion);
    g_task_set_task_data(task, input, InputFree); g_task_run_in_thread(task, Worker); g_object_unref(task);
    return UMI_STATUS_OK;
}
static void RefreshClicked(GtkButton *button, gpointer data)
{ (void)button; (void)UmiDesktopSystemGtk4Refresh(GTK_WIDGET(data)); }
GtkWidget *UmiDesktopSystemGtk4PanelNew(void)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    SystemView *view = g_new0(SystemView, 1);
    g_object_set_data_full(G_OBJECT(root), "umicom-system-view", view, g_free);
    gtk_widget_set_margin_top(root, 16); gtk_widget_set_margin_bottom(root, 16);
    gtk_widget_set_margin_start(root, 16); gtk_widget_set_margin_end(root, 16);
    gtk_box_append(GTK_BOX(root), Brand(UMICOM_SYSTEM_LOGO, sizeof UMICOM_SYSTEM_LOGO, 36));
    GtkWidget *title = gtk_label_new("System Centre"); gtk_widget_add_css_class(title, "title-2");
    gtk_label_set_xalign(GTK_LABEL(title), 0); gtk_box_append(GTK_BOX(root), title);
    GtkWidget *intro = gtk_label_new("Inspect this host without changing it. Choose Refresh when you need a new observation.");
    gtk_label_set_wrap(GTK_LABEL(intro), TRUE); gtk_label_set_xalign(GTK_LABEL(intro), 0); gtk_box_append(GTK_BOX(root), intro);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(row), gtk_label_new("Filesystem path")); view->storage = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(view->storage), "Leave blank for the local root volume");
    gtk_widget_set_hexpand(view->storage, TRUE); gtk_box_append(GTK_BOX(row), view->storage);
    view->refresh = gtk_button_new_with_label("Refresh"); gtk_widget_set_name(view->refresh, "umicom-system-refresh");
    g_signal_connect_object(view->refresh, "clicked", G_CALLBACK(RefreshClicked), root, 0); gtk_box_append(GTK_BOX(row), view->refresh);
    gtk_box_append(GTK_BOX(root), row);
    view->summary = gtk_label_new("No capture yet. No data is saved automatically.");
    gtk_label_set_wrap(GTK_LABEL(view->summary), TRUE); gtk_label_set_xalign(GTK_LABEL(view->summary), 0);
    gtk_box_append(GTK_BOX(root), view->summary);
    GtkWidget *notebook = gtk_notebook_new(); gtk_widget_set_vexpand(notebook, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), Page(&view->overview), gtk_label_new("Overview"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), Page(&view->processes), gtk_label_new("Processes"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), Page(&view->network), gtk_label_new("Network"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), Page(&view->boot), gtk_label_new("Boot checks"));
    gtk_box_append(GTK_BOX(root), notebook); return root;
}
static void WeakFree(gpointer data)
{ GWeakRef *reference = data; g_weak_ref_clear(reference); g_free(reference); }
static void OpenCentre(GtkButton *button, gpointer unused)
{
    (void)unused; GWeakRef *reference = g_object_get_data(G_OBJECT(button), "umicom-system-window");
    GtkWindow *window = g_weak_ref_get(reference);
    if (window) { gtk_window_present(window); g_object_unref(window); return; }
    window = GTK_WINDOW(gtk_window_new()); gtk_window_set_title(window, "Umicom System Centre");
    gtk_window_set_default_size(window, 860, 640);
    GtkRoot *root = gtk_widget_get_root(GTK_WIDGET(button));
    if (root && GTK_IS_WINDOW(root)) {
        GtkApplication *app = gtk_window_get_application(GTK_WINDOW(root));
        if (app) gtk_window_set_application(window, app);
        gtk_window_set_transient_for(window, GTK_WINDOW(root)); gtk_window_set_destroy_with_parent(window, TRUE);
    }
    gtk_window_set_icon_name(window, "org.umicom.desktop");
    gtk_window_set_child(window, UmiDesktopSystemGtk4PanelNew());
    g_weak_ref_set(reference, window); gtk_window_present(window);
}
UmiStatus UmiDesktopSystemGtk4Attach(GtkWidget *rootBox)
{
    if (!rootBox || !GTK_IS_BOX(rootBox)) return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *button = gtk_button_new(); GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_box_append(GTK_BOX(row), Brand(UMICOM_SYSTEM_ICON, sizeof UMICOM_SYSTEM_ICON, 20));
    gtk_box_append(GTK_BOX(row), gtk_label_new("Open System Centre")); gtk_button_set_child(GTK_BUTTON(button), row);
    gtk_widget_set_halign(button, GTK_ALIGN_START);
    gtk_widget_set_tooltip_text(button, "Read-only memory, process, filesystem, network and initial boot observations.");
    GWeakRef *reference = g_new0(GWeakRef, 1); g_weak_ref_init(reference, NULL);
    g_object_set_data_full(G_OBJECT(button), "umicom-system-window", reference, WeakFree);
    g_signal_connect(button, "clicked", G_CALLBACK(OpenCentre), NULL); gtk_box_append(GTK_BOX(rootBox), button);
    return UMI_STATUS_OK;
}

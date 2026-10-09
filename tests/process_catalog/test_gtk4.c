/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/process_catalog/test_gtk4.c
 * PURPOSE: Exercise asynchronous process selection, cancellation and retained controls with a controlled metadata provider.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/process_picker.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#define CHECK(v)                                                                                   \
    do                                                                                             \
    {                                                                                              \
        if (!(v))                                                                                  \
        {                                                                                          \
            fprintf(stderr, "%d: %s\n", __LINE__, #v);                                             \
            failed = 1;                                                                            \
            goto done;                                                                             \
        }                                                                                          \
    } while (0)
typedef struct Host
{
    gint captures, provider_destroyed, finished;
    unsigned chosen, chosen_destroyed;
    uint64_t pid;
    bool denied, slow, fixture, unknown, bad;
    GtkWindow *window;
    bool close_choose;
} Host;
static UmiStatus Capture(void *context, const UmiDesktopProcessCaptureOptions *options,
                         UmiDesktopProcessCatalog **out)
{
    Host *host = context;
    g_atomic_int_inc(&host->captures);
    *out = NULL;
    if (host->slow)
        for (unsigned i = 0U; i < 200U; ++i)
        {
            if (options->cancelled(options->context))
            {
                g_atomic_int_inc(&host->finished);
                return UMI_STATUS_CANCELLED;
            }
            g_usleep(1000U);
        }
    UmiStatus status = UMI_STATUS_IO_ERROR;
    if (!host->bad)
    {
        UmiDesktopSystemProcess rows[25] = {0};
        for (size_t i = 0U; i < 25U; ++i)
        {
            rows[i].pid = 100U + i;
            rows[i].startTicks = 200U + i;
            rows[i].startKnown = !host->unknown;
            g_snprintf(rows[i].name, sizeof rows[i].name, i == 24U ? "Editor" : "worker-%zu", i);
        }
        UmiDesktopProcessReport report = {.seen = 25, .fixture = host->fixture, .limited = 1};
        status = UmiDesktopProcessCatalogCreate(rows, 25, &report, out);
    }
    g_atomic_int_inc(&host->finished);
    return status;
}
static UmiStatus Choose(void *context, const UmiDesktopSystemProcess *process)
{
    Host *host = context;
    ++host->chosen;
    host->pid = process->pid;
    if (host->close_choose)
        gtk_window_destroy(host->window);
    return host->denied ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_OK;
}
static void ProviderFree(gpointer context)
{
    g_atomic_int_inc(&((Host *)context)->provider_destroyed);
}
static void ChooseFree(gpointer context) { ++((Host *)context)->chosen_destroyed; }
static void Drain(void)
{
    for (unsigned i = 0U; i < 30U; ++i)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
}
static bool Ready(GtkWidget *refresh)
{
    gint64 until = g_get_monotonic_time() + 5000000;
    while (!gtk_widget_get_sensitive(refresh) && g_get_monotonic_time() < until)
        Drain();
    return gtk_widget_get_sensitive(refresh);
}
static GtkWidget *Find(GtkWidget *panel, const char *tag)
{
    return umi_gtk4_automation_find_tagged_widget(panel, tag);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"normal",         "search", "page",        "old-row", "fixture",
                           "unknown",        "cancel", "close",       "hidden",  "retained",
                           "failed-refresh", "denied", "close-choose"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    Host host = {0};
    host.fixture = strcmp(mode, "fixture") == 0;
    host.unknown = strcmp(mode, "unknown") == 0;
    host.slow = strcmp(mode, "cancel") == 0 || strcmp(mode, "close") == 0;
    host.denied = strcmp(mode, "denied") == 0;
    host.close_choose = strcmp(mode, "close-choose") == 0;
    UmiGtk4ProcessProvider provider = {Capture, &host, ProviderFree};
    GtkWidget *panel = g_object_ref_sink(
                  UmiGtk4ProcessPickerCreate(&provider, Choose, &host, ChooseFree)),
              *retained = NULL;
    host.window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    gtk_window_set_child(host.window, panel);
    gtk_window_present(host.window);
    Drain();
    GtkWidget *refresh = Find(panel, "process.picker.refresh"),
              *search = Find(panel, "process.picker.search");
    CHECK(refresh != NULL && search != NULL && g_atomic_int_get(&host.captures) == 0);
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(host.window);
    g_signal_emit_by_name(refresh, "clicked");
    if (strcmp(mode, "hidden") == 0 || strcmp(mode, "retained") == 0)
    {
        Drain();
        CHECK(g_atomic_int_get(&host.captures) == 0);
        goto done;
    }
    if (strcmp(mode, "cancel") == 0)
    {
        g_signal_emit_by_name(Find(panel, "process.picker.stop"), "clicked");
        CHECK(Ready(refresh) && Find(panel, "process.picker.choose.0") == NULL);
        goto done;
    }
    if (strcmp(mode, "close") == 0)
    {
        gtk_window_destroy(host.window);
        g_clear_object(&panel);
        Drain();
        goto done;
    }
    CHECK(Ready(refresh));
    GtkWidget *choice = Find(panel, "process.picker.choose.0");
    CHECK(choice != NULL &&
          strstr(gtk_label_get_text(GTK_LABEL(Find(panel, "process.picker.status"))),
                 "incomplete") != NULL);
    if (host.fixture || host.unknown)
    {
        CHECK(!gtk_widget_get_sensitive(choice));
        g_signal_emit_by_name(choice, "clicked");
        CHECK(host.chosen == 0U);
        goto done;
    }
    if (strcmp(mode, "search") == 0 || strcmp(mode, "old-row") == 0)
    {
        retained = g_object_ref(choice);
        gtk_editable_set_text(GTK_EDITABLE(search), "EDITOR");
        choice = Find(panel, "process.picker.choose.0");
        CHECK(choice != NULL);
        g_signal_emit_by_name(retained, "clicked");
        CHECK(host.chosen == 0U);
    }
    if (strcmp(mode, "page") == 0)
    {
        g_signal_emit_by_name(Find(panel, "process.picker.next"), "clicked");
        choice = Find(panel, "process.picker.choose.0");
        CHECK(choice != NULL);
    }
    if (strcmp(mode, "failed-refresh") == 0)
    {
        host.bad = true;
        g_signal_emit_by_name(refresh, "clicked");
        CHECK(Find(panel, "process.picker.choose.0") == NULL && Ready(refresh));
        CHECK(Find(panel, "process.picker.choose.0") == NULL);
        goto done;
    }
    Drain();
    g_signal_emit_by_name(choice, "clicked");
    CHECK(host.chosen == 1U &&
          host.pid == (strcmp(mode, "page") == 0                                       ? 120U
                       : (strcmp(mode, "search") == 0 || strcmp(mode, "old-row") == 0) ? 124U
                                                                                       : 100U));
done:
    g_clear_object(&retained);
    gtk_window_destroy(host.window);
    g_clear_object(&panel);
    g_object_unref(host.window);
    /* A worker owns its provider until cancellation has unwound. Keep this test
     * context alive until its final destroy callback has been observed. */
    gint64 until = g_get_monotonic_time() + 5000000;
    while (!g_atomic_int_get(&host.provider_destroyed) && g_get_monotonic_time() < until)
        Drain();
    if (g_atomic_int_get(&host.provider_destroyed) != 1 || host.chosen_destroyed != 1U)
        failed = 1;
    return failed;
}

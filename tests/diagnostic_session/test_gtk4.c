/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/diagnostic_session/test_gtk4.c
 * PURPOSE: Exercise live source changes, authorization revocation and retained diagnostic controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/path.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/live_diagnostics.h"
#include <stdio.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
typedef struct Host
{
    char source[256];
    bool allowed, fail;
    unsigned captures, navigations;
    GtkWindow *navigation_close;
    bool navigation_failed;
    GtkWindow *close;
    GtkWidget *reenter;
} Host;
static bool Authorized(void *data) { return ((Host *)data)->allowed; }
static UmiStatus Capture(char *out, size_t capacity, size_t *bytes, void *data)
{
    Host *host = data;
    ++host->captures;
    if (host->reenter != NULL)
        g_signal_emit_by_name(host->reenter, "clicked");
    if (host->close != NULL)
        gtk_window_destroy(host->close);
    if (host->fail)
        return UMI_STATUS_NOT_FOUND;
    size_t length = strlen(host->source);
    if (length >= capacity)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, host->source, length + 1U);
    *bytes = length;
    return UMI_STATUS_OK;
}
static UmiStatus Navigate(const char *uri, const char *source, size_t bytes,
                          UmiEditorTextPosition first, UmiEditorTextPosition last, void *data)
{
    Host *host = data;
    ++host->navigations;
    if (host->navigation_close != NULL)
        gtk_window_destroy(host->navigation_close);
    /* Read borrowed values after closing to exercise the panel's independent
     * callback argument ownership, not only the normal visible path. */
    host->navigation_failed =
        strcmp(uri, "file:///workspace/main.c") != 0 || bytes != strlen(host->source) ||
        memcmp(source, host->source, bytes) != 0 || first.line != 0U || first.utf16_column != 0U ||
        last.line != 0U || last.utf16_column != 0U;
    return host->navigation_failed ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
static void Drain(void)
{
    for (unsigned i = 0U; i < 40U; ++i)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
}
static bool Contains(GtkWidget *view, const char *needle)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    bool found = strstr(text, needle) != NULL;
    g_free(text);
    return found;
}
static bool Wait(GtkWidget *panel, GtkWidget *output, const char *needle)
{
    for (unsigned i = 0U; i < 12000U; ++i)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        if (needle == NULL ? !UmiGtk4LiveDiagnosticPanelPending(panel) : Contains(output, needle))
            return true;
        g_usleep(1000U);
    }
    return false;
}
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    g_setenv("GTK_A11Y", "test", TRUE);
    g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    GtkWindow *window = NULL;
    GtkWidget *panel = NULL;
    Host host = {.allowed = true};
    strcpy(host.source, "initial source");
    UmiLanguageDiagnosticMonitorConfig config = {0};
    strcpy(config.profile.id, "native-fixture");
    config.profile.enabled = 1;
    CHECK(strlen(argv[2]) < sizeof config.profile.executable);
    strcpy(config.profile.executable, argv[2]);
    char directory[UMI_PATH_CAPACITY];
    CHECK(umi_path_parent(argv[2], directory, sizeof directory) == UMI_STATUS_OK);
    config.working_directory = directory;
    config.document =
        (UmiLanguageDiagnosticRequest){"file:///workspace", "file:///workspace/main.c", "c",
                                       host.source,         strlen(host.source),        5000U};
    UmiGtk4LiveDiagnosticCallbacks callbacks = {Capture, Authorized, &host, NULL};
    CHECK(UmiGtk4LiveDiagnosticPanelCreate(&config, &callbacks, &panel) == UMI_STATUS_OK);
    CHECK(UmiGtk4LiveDiagnosticPanelSetNavigator(panel, Navigate) == UMI_STATUS_OK);
    g_object_ref_sink(panel);
    window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    gtk_window_set_child(window, panel);
    gtk_window_present(window);
    Drain();
    GtkWidget *start = umi_gtk4_automation_find_tagged_widget(panel, "live.diagnostics.start");
    GtkWidget *stop = umi_gtk4_automation_find_tagged_widget(panel, "live.diagnostics.stop");
    GtkWidget *output = umi_gtk4_automation_find_tagged_widget(panel, "live.diagnostics.output");
    CHECK(start != NULL && stop != NULL && output != NULL && host.captures == 0U);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(window);
    if (strcmp(mode, "denied") == 0)
        host.allowed = false;
    if (strcmp(mode, "capture-close") == 0)
        host.close = window;
    if (strcmp(mode, "reentrant") == 0)
        host.reenter = start;
    g_signal_emit_by_name(start, "clicked");
    if (strcmp(mode, "retained") == 0 || strcmp(mode, "denied") == 0 ||
        strcmp(mode, "capture-close") == 0)
    {
        CHECK(!UmiGtk4LiveDiagnosticPanelPending(panel));
        goto cleanup;
    }
    CHECK(UmiGtk4LiveDiagnosticPanelPending(panel));
    CHECK(Wait(panel, output, "initial source"));
    CHECK(UmiGtk4LiveDiagnosticPanelSetNavigator(panel, NULL) == UMI_STATUS_BUSY);
    if (strncmp(mode, "navigate", 8U) == 0)
    {
        GtkWidget *open = umi_gtk4_automation_find_tagged_widget(panel, "live.diagnostics.open");
        CHECK(open != NULL && gtk_widget_get_sensitive(open));
        if (strcmp(mode, "navigate-stale") == 0)
            strcpy(host.source, "changed before poll");
        else if (strcmp(mode, "navigate-denied") == 0)
            host.allowed = false;
        else if (strcmp(mode, "navigate-close") == 0)
            host.navigation_close = window;
        else
            CHECK(strcmp(mode, "navigate") == 0);
        g_signal_emit_by_name(open, "clicked");
        bool refused = strcmp(mode, "navigate-stale") == 0 || strcmp(mode, "navigate-denied") == 0;
        CHECK(host.navigations == (refused ? 0U : 1U) && !host.navigation_failed);
        if (refused)
            CHECK(!Contains(output, "initial source"));
        if (strcmp(mode, "navigate-close") != 0)
            g_signal_emit_by_name(stop, "clicked");
        CHECK(Wait(panel, output, NULL));
        goto cleanup;
    }

    if (strcmp(mode, "update") == 0 || strcmp(mode, "reentrant") == 0)
    {
        strcpy(host.source, "changed \xf0\x9f\x98\x80");
        CHECK(Wait(panel, output, host.source));
        CHECK(!Contains(output, "initial source"));
        g_signal_emit_by_name(stop, "clicked");
    }
    else if (strcmp(mode, "revoke") == 0)
        host.allowed = false;
    else if (strcmp(mode, "document-close") == 0)
        host.fail = true;
    else if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    else
    {
        CHECK(strcmp(mode, "close") == 0);
        gtk_window_destroy(window);
    }
    CHECK(Wait(panel, output, NULL));
    if (strcmp(mode, "hidden") == 0)
    {
        gtk_widget_set_visible(panel, TRUE);
        Drain();
        CHECK(gtk_widget_get_sensitive(start));
        CHECK(!Contains(output, "initial source"));
    }
    else if (strcmp(mode, "close") != 0)
        CHECK(!Contains(output, "initial source"));
cleanup:
    if (window != NULL)
        gtk_window_destroy(window);
    if (panel != NULL)
        (void)Wait(panel, NULL, NULL);
    g_clear_object(&window);
    g_clear_object(&panel);
    return failed;
}

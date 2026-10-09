/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/program_console/test_gtk4.c
 * PURPOSE: Exercise native input, authority revocation and retained console controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/path.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/program_console.h"
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
    const char *program;
    char directory[UMI_PATH_CAPACITY];
    bool allowed;
    unsigned prepared;
    GtkWindow *close_on_prepare;
    GtkWidget *reenter_run;
} Host;
static bool Authorized(void *context) { return ((Host *)context)->allowed; }
static UmiStatus Prepare(UmiProgramConsole **out, void *context)
{
    Host *host = context;
    ++host->prepared;
    if (host->reenter_run != NULL)
        g_signal_emit_by_name(host->reenter_run, "clicked");
    const char *argument = "echo";
    UmiProgramConsoleConfig config = {0};
    config.command.program = host->program;
    config.command.arguments = &argument;
    config.command.argumentCount = 1U;
    config.command.workingDirectory = host->directory;
    config.timeout_ms = 5000U;
    UmiStatus status = UmiProgramConsoleCreate(&config, out);
    if (host->close_on_prepare != NULL)
        gtk_window_destroy(host->close_on_prepare);
    return status;
}
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
static bool Wait(GtkWidget *panel)
{
    for (unsigned i = 0U; i < 10000U; ++i)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        if (!UmiGtk4ProgramConsolePanelPending(panel))
            return true;
        g_usleep(1000U);
    }
    return false;
}
static bool TextContains(GtkWidget *view, const char *needle)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    bool found = strstr(text, needle) != NULL;
    g_free(text);
    return found;
}
int main(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    const char *known[] = {"input",  "retained",  "close",     "hidden",
                           "revoke", "untrusted", "reentrant", "prepare-close"};
    bool valid = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i)
        if (strcmp(mode, known[i]) == 0)
            valid = true;
    if (!valid)
        return 2;
    g_setenv("GTK_A11Y", "test", TRUE);
    g_setenv("GSETTINGS_BACKEND", "memory", TRUE);
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    Host host = {0};
    host.program = argv[2];
    host.allowed = strcmp(mode, "untrusted") != 0;
    GtkWindow *window = NULL;
    GtkWidget *panel = NULL;
    CHECK(umi_path_parent(argv[2], host.directory, sizeof host.directory) == UMI_STATUS_OK);
    UmiGtk4ProgramConsoleCallbacks callbacks = {Prepare, Authorized, &host, NULL};
    CHECK(UmiGtk4ProgramConsolePanelCreate(&callbacks, &panel) == UMI_STATUS_OK);
    g_object_ref_sink(panel);
    window = GTK_WINDOW(gtk_window_new());
    g_object_ref(window);
    gtk_window_set_child(window, panel);
    gtk_window_present(window);
    Drain();
    GtkWidget *run = umi_gtk4_automation_find_tagged_widget(panel, "program.console.run");
    GtkWidget *send = umi_gtk4_automation_find_tagged_widget(panel, "program.console.send");
    GtkWidget *input = umi_gtk4_automation_find_tagged_widget(panel, "program.console.input");
    GtkWidget *end = umi_gtk4_automation_find_tagged_widget(panel, "program.console.end");
    GtkWidget *output = umi_gtk4_automation_find_tagged_widget(panel, "program.console.output");
    CHECK(run != NULL && send != NULL && input != NULL && end != NULL && output != NULL);
    CHECK(host.prepared == 0U && !UmiGtk4ProgramConsolePanelPending(panel));
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(window);
    if (strcmp(mode, "prepare-close") == 0)
        host.close_on_prepare = window;
    if (strcmp(mode, "reentrant") == 0)
        host.reenter_run = run;
    g_signal_emit_by_name(run, "clicked");
    if (strcmp(mode, "retained") == 0 || strcmp(mode, "untrusted") == 0)
    {
        CHECK(host.prepared == 0U && !UmiGtk4ProgramConsolePanelPending(panel));
        goto cleanup;
    }
    CHECK(host.prepared == 1U && UmiGtk4ProgramConsolePanelPending(panel));
    if (strcmp(mode, "close") == 0)
        gtk_window_destroy(window);
    else if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    else if (strcmp(mode, "revoke") == 0)
        host.allowed = false;
    else if (strcmp(mode, "prepare-close") != 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(input), "caf\xc3\xa9 console");
        g_signal_emit_by_name(send, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(input)), "") == 0);
        g_signal_emit_by_name(end, "clicked");
    }
    CHECK(Wait(panel));
    if (strcmp(mode, "input") == 0 || strcmp(mode, "reentrant") == 0)
    {
        CHECK(TextContains(output, "caf\xc3\xa9 console\n") && TextContains(output, "EOF"));
        CHECK(!gtk_widget_get_sensitive(send));
    }
    if (strcmp(mode, "hidden") == 0)
    {
        gtk_widget_set_visible(panel, TRUE);
        Drain();
        CHECK(gtk_widget_get_sensitive(run));
    }
cleanup:
    if (window != NULL)
        gtk_window_destroy(window);
    if (panel != NULL)
        (void)Wait(panel);
    g_clear_object(&window);
    g_clear_object(&panel);
    return failed;
}

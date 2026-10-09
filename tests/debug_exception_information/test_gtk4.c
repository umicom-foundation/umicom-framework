/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exception_information/test_gtk4.c
 * PURPOSE: Check captured exception presentation and window-lifetime boundaries without a debugger process.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_exception_information.h"
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
    GtkWindow *window;
    unsigned calls, destroyed;
    bool denied, close_query, invalid;
} Host;
static UmiStatus Query(void *context, UmiDebugExceptionTarget *target,
                       UmiDebugExceptionInformation *out)
{
    Host *host = context;
    ++host->calls;
    if (host->denied)
        return UMI_STATUS_PERMISSION_DENIED;
    memset(target, 0, sizeof *target);
    strcpy(target->connection.session_id, "session");
    target->connection.generation = 1U;
    target->revision = 7U;
    target->thread_id = 1U;
    memset(out, 0, sizeof *out);
    strcpy(out->exception_id, "fixture-error");
    strcpy(out->break_mode, "always");
    out->count = 2U;
    out->details[0].parent = SIZE_MAX;
    strcpy(out->details[0].message, "outer");
    strcpy(out->details[0].evaluate_name, "must_not_execute()");
    strcpy(out->details[0].stack_trace, "<script>text()</script>\n");
    out->details[1].parent = host->invalid ? 1U : 0U;
    out->details[1].depth = 1U;
    strcpy(out->details[1].message, "inner");
    if (host->close_query)
        gtk_window_destroy(host->window);
    return UMI_STATUS_OK;
}
static void Destroy(gpointer context) { ++((Host *)context)->destroyed; }
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
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"normal",      "denied",       "hidden",        "retained",
                           "close-query", "invalid-host", "failed-refresh"};
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
    host.denied = strcmp(mode, "denied") == 0;
    host.close_query = strcmp(mode, "close-query") == 0;
    host.invalid = strcmp(mode, "invalid-host") == 0;
    GtkWidget *panel =
        g_object_ref_sink(UmiGtk4DebugExceptionInformationCreate(Query, &host, Destroy));
    host.window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    gtk_window_set_child(host.window, panel);
    gtk_window_present(host.window);
    Drain();
    GtkWidget *refresh = umi_gtk4_automation_find_tagged_widget(panel, "debug.exception.refresh");
    CHECK(refresh != NULL);
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(host.window);
    g_signal_emit_by_name(refresh, "clicked");
    if (strcmp(mode, "hidden") == 0 || strcmp(mode, "retained") == 0)
    {
        CHECK(host.calls == 0U);
        goto done;
    }
    CHECK(host.calls == 1U);
    GtkWidget *trace = umi_gtk4_automation_find_tagged_widget(panel, "debug.exception.trace.0");
    if (host.denied || host.invalid)
    {
        CHECK(trace == NULL);
        goto done;
    }
    CHECK(trace != NULL && !gtk_text_view_get_editable(GTK_TEXT_VIEW(trace)));
    GtkTextIter first, last;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(trace));
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    bool matches = strcmp(text, "<script>text()</script>\n") == 0;
    g_free(text);
    CHECK(matches);
    if (strcmp(mode, "failed-refresh") == 0)
    {
        host.denied = true;
        g_signal_emit_by_name(refresh, "clicked");
        CHECK(host.calls == 2U);
        CHECK(umi_gtk4_automation_find_tagged_widget(panel, "debug.exception.trace.0") == NULL);
    }
done:
    gtk_window_destroy(host.window);
    g_object_unref(host.window);
    g_object_unref(panel);
    if (host.destroyed != 1U)
    {
        fprintf(stderr, "Context destruction count: %u\n", host.destroyed);
        failed = 1;
    }
    return failed;
}

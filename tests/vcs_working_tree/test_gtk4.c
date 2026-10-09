/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_gtk4.c
 * PURPOSE: Verify repository review pagination, failed refresh preservation and closed control
 * lifetime. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/platform/threading.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/vcs/working_tree_gtk4.h"
#include <gtk/gtk.h>
#include <stdatomic.h>

static atomic_int entered, released, exited;
static int fail_read;
/* Gate the read at a deterministic point so closing a window cannot race an already-finished
 * fixture. */
UmiStatus UmiVcsWorkingTreeRead(const UmiVcsWorkingTreeRequest *request, UmiVcsWorkingTree **out)
{
    CHECK(strcmp(request->repository_root, "review-root") == 0);
    atomic_store(&entered, 1);
    while (!atomic_load(&released))
    {
        if (umi_cancellation_token_is_requested(request->cancellation))
        {
            atomic_store(&exited, 1);
            return UMI_STATUS_CANCELLED;
        }
        umi_thread_sleep_ms(1U);
    }
    UmiStatus status;
    if (fail_read)
        status = UMI_STATUS_IO_ERROR;
    else
    {
        GString *bytes = g_string_new_len(HEADERS, sizeof(HEADERS) - 1U);
        g_string_append_len(bytes, CHILD "framework\0", sizeof(CHILD "framework\0") - 1U);
        for (unsigned index = 0U; index < 60U; ++index)
        {
            g_string_append_printf(bytes, "? source-%u.c", index);
            g_string_append_c(bytes, '\0');
        }
        status = UmiVcsWorkingTreeParse(bytes->str, bytes->len, out);
        g_string_free(bytes, TRUE);
    }
    atomic_store(&exited, 1);
    return status;
}
/* Pump only for bounded fixture observations, never as a substitute for production lifetime
 * ownership. */
static void Pump(void)
{
    while (g_main_context_iteration(NULL, FALSE))
    {
    }
    umi_thread_sleep_ms(1U);
}
static void AwaitFlag(atomic_int *flag)
{
    for (unsigned index = 0U; index < 5000U && !atomic_load(flag); ++index)
        Pump();
    CHECK(atomic_load(flag));
}
static void AwaitEnabled(GtkWidget *widget)
{
    for (unsigned index = 0U; index < 5000U && !gtk_widget_get_sensitive(widget); ++index)
        Pump();
    CHECK(gtk_widget_get_sensitive(widget));
}
static char *ReadOutput(GtkWidget *output)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(output));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    return gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!gtk_init_check() || !UmiThreadCanTryJoin())
        return 77;
    const char *mode = argv[1];
    void *native = NULL;
    atomic_init(&entered, 0);
    atomic_init(&released, 0);
    atomic_init(&exited, 0);
    CHECK(UmiVcsWorkingTreeGtk4Create("review-root", &native) == UMI_STATUS_OK);
    GtkWidget *panel = native;
    GtkWidget *window = gtk_window_new();
    g_object_ref_sink(window);
    gtk_window_set_child(GTK_WINDOW(window), panel);
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 500);
    GtkWidget *inspect = umi_gtk4_automation_find_tagged_widget(panel, "vcs.review.inspect");
    GtkWidget *stop = umi_gtk4_automation_find_tagged_widget(panel, "vcs.review.stop");
    GtkWidget *output = umi_gtk4_automation_find_tagged_widget(panel, "vcs.review.output");
    GtkWidget *next = umi_gtk4_automation_find_tagged_widget(panel, "vcs.review.next");
    GtkWidget *previous = umi_gtk4_automation_find_tagged_widget(panel, "vcs.review.previous");
    CHECK(inspect && stop && output && next && previous);
    CHECK(!gtk_widget_get_sensitive(stop) && !gtk_text_view_get_editable(GTK_TEXT_VIEW(output)));
    gtk_window_present(GTK_WINDOW(window));
    for (unsigned index = 0U; index < 5000U && !gtk_widget_get_mapped(panel); ++index)
        Pump();
    CHECK(gtk_widget_get_mapped(panel));
    g_signal_emit_by_name(inspect, "clicked");
    AwaitFlag(&entered);
    CHECK(!gtk_widget_get_sensitive(inspect) && gtk_widget_get_sensitive(stop));
    if (strcmp(mode, "close-retained") == 0)
    {
        g_object_ref(inspect);
        gtk_window_destroy(GTK_WINDOW(window));
        g_object_unref(window);
        AwaitFlag(&exited);
        /* A retained button must neither access freed panel state nor launch another job. */
        atomic_store(&entered, 0);
        g_signal_emit_by_name(inspect, "clicked");
        for (unsigned index = 0U; index < 100U; ++index)
            Pump();
        CHECK(!atomic_load(&entered));
        g_object_unref(inspect);
        return 0;
    }
    if (strcmp(mode, "stop") == 0)
        g_signal_emit_by_name(stop, "clicked");
    else
        atomic_store(&released, 1);
    AwaitFlag(&exited);
    AwaitEnabled(inspect);
    char *text = ReadOutput(output);
    if (strcmp(mode, "stop") == 0)
        CHECK(text[0] == '\0');
    else
    {
        CHECK(strstr(text, "Nothing is staged here") && strstr(text, "Paths 1 through 50 of 61"));
        if (strcmp(mode, "paging") == 0)
        {
            g_signal_emit_by_name(next, "clicked");
            g_free(text);
            text = ReadOutput(output);
            CHECK(strstr(text, "Paths 51 through 61 of 61"));
            CHECK(!gtk_widget_get_sensitive(next) && gtk_widget_get_sensitive(previous));
            g_signal_emit_by_name(previous, "clicked");
            CHECK(!gtk_widget_get_sensitive(previous));
        }
        else if (strcmp(mode, "failed-refresh") == 0)
        {
            fail_read = 1;
            atomic_store(&entered, 0);
            atomic_store(&exited, 0);
            g_signal_emit_by_name(inspect, "clicked");
            AwaitFlag(&exited);
            AwaitEnabled(inspect);
            char *after = ReadOutput(output);
            CHECK(strcmp(text, after) == 0);
            g_free(after);
        }
        else
            CHECK(strcmp(mode, "render") == 0);
    }
    g_free(text);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
    return 0;
}

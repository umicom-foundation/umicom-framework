/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_sources/test_gtk4.c
 * PURPOSE: Check source browsing, filtering, read-only presentation and stale or retained control refusal.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_sources.h"
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
typedef struct SourceHost
{
    GtkWindow *window;
    unsigned refreshes, reads, destroyed;
    uint64_t generation;
    bool deny_refresh, deny_read, close_read, invalid, markup;
} SourceHost;
static UmiStatus Refresh(void *context, UmiDebugSourceCatalog *out)
{
    SourceHost *host = context;
    ++host->refreshes;
    if (host->deny_refresh)
        return UMI_STATUS_PERMISSION_DENIED;
    memset(out, 0, sizeof *out);
    strcpy(out->connection.session_id, "source-session");
    out->connection.generation = host->generation;
    out->count = 18U;
    for (size_t i = 0U; i < out->count; ++i)
    {
        snprintf(out->items[i].name, sizeof out->items[i].name, "file-%zu.c", i);
        strcpy(out->items[i].path, "/remote/path.c");
        out->items[i].reference = (uint32_t)i + 1U;
    }
    out->items[1].reference = 0U;
    if (host->invalid)
        out->count = UMI_DEBUG_SOURCE_CATALOG_LIMIT + 1U;
    return UMI_STATUS_OK;
}
static UmiStatus Read(void *context, const UmiDebugConnectionIdentity *connection,
                      uint32_t reference, UmiDebugSourceContent *out)
{
    SourceHost *host = context;
    ++host->reads;
    if (host->deny_read)
        return UMI_STATUS_PERMISSION_DENIED;
    if (connection->generation != host->generation)
        return UMI_STATUS_INVALID_STATE;
    if (reference != 1U)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    strcpy(out->text, host->markup ? "<script>alert(1)</script>" : "int value = 42;\n");
    strcpy(out->mime_type, host->markup ? "text/html" : "text/x-c");
    out->bytes = strlen(out->text);
    if (host->close_read)
        gtk_window_destroy(host->window);
    return UMI_STATUS_OK;
}
static void Destroy(gpointer context) { ++((SourceHost *)context)->destroyed; }
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
static char *Text(GtkWidget *viewer)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(viewer));
    GtkTextIter first, last;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"normal",  "filter",      "pages",       "path-only", "markup",
                           "denied",  "read-denied", "stale",       "hidden",    "retained",
                           "old-row", "close-read",  "invalid-host"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    SourceHost host = {0};
    host.generation = 1U;
    host.deny_refresh = strcmp(mode, "denied") == 0;
    host.deny_read = strcmp(mode, "read-denied") == 0;
    host.close_read = strcmp(mode, "close-read") == 0;
    host.invalid = strcmp(mode, "invalid-host") == 0;
    host.markup = strcmp(mode, "markup") == 0;
    GtkWidget *panel = g_object_ref_sink(UmiGtk4DebugSourcesCreate(Refresh, Read, &host, Destroy));
    GtkWidget *held = NULL;
    char *text = NULL;
    host.window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    gtk_window_set_default_size(host.window, 850, 1000);
    gtk_window_set_child(host.window, panel);
    gtk_window_present(host.window);
    Drain();
    GtkWidget *refresh = umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.refresh");
    GtkWidget *viewer = umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.text");
    CHECK(refresh != NULL && viewer != NULL && !gtk_text_view_get_editable(GTK_TEXT_VIEW(viewer)));
    g_signal_emit_by_name(refresh, "clicked");
    Drain();
    CHECK(host.refreshes == 1U);
    if (host.deny_refresh || host.invalid)
    {
        CHECK(umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.view.0") == NULL);
        goto done;
    }
    GtkWidget *view = umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.view.0");
    CHECK(view != NULL);
    held = g_object_ref(view);
    if (strcmp(mode, "pages") == 0)
    {
        GtkWidget *next = umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.next");
        GtkWidget *previous =
            umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.previous");
        CHECK(next != NULL && previous != NULL && gtk_widget_get_sensitive(next));
        g_signal_emit_by_name(next, "clicked");
        CHECK(umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.view.16") != NULL &&
              !gtk_widget_get_sensitive(next));
        g_signal_emit_by_name(previous, "clicked");
        CHECK(umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.view.0") != NULL);
        CHECK(host.refreshes == 1U && host.reads == 0U);
        goto done;
    }
    if (strcmp(mode, "filter") == 0)
    {
        GtkWidget *search = umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.search");
        CHECK(search != NULL);
        gtk_editable_set_text(GTK_EDITABLE(search), "FILE-17.C");
        CHECK(umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.view.17") != NULL);
        CHECK(umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.view.0") == NULL);
        CHECK(host.refreshes == 1U && host.reads == 0U);
        goto done;
    }
    if (strcmp(mode, "path-only") == 0)
    {
        GtkWidget *local = umi_gtk4_automation_find_tagged_widget(panel, "debug.sources.view.1");
        CHECK(local != NULL && !gtk_widget_get_sensitive(local));
        g_signal_emit_by_name(local, "clicked");
        CHECK(host.reads == 0U);
        goto done;
    }
    if (strcmp(mode, "stale") == 0)
        ++host.generation;
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(host.window);
    if (strcmp(mode, "old-row") == 0)
    {
        g_signal_emit_by_name(refresh, "clicked");
        Drain();
    }
    g_signal_emit_by_name(held, "clicked");
    bool blocked = strcmp(mode, "hidden") == 0 || strcmp(mode, "retained") == 0 ||
                   strcmp(mode, "old-row") == 0;
    CHECK(host.reads == (blocked ? 0U : 1U));
    text = Text(viewer);
    if (blocked || host.deny_read || strcmp(mode, "stale") == 0)
        CHECK(text[0] == '\0');
    else
        CHECK(strcmp(text, host.markup ? "<script>alert(1)</script>" : "int value = 42;\n") == 0);
done:
    gtk_window_destroy(host.window);
    g_clear_object(&held);
    g_object_unref(host.window);
    g_object_unref(panel);
    if (host.destroyed != 1U)
    {
        fprintf(stderr, "Context was not destroyed exactly once\n");
        failed = 1;
    }
    g_free(text);
    return failed;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_modules/test_gtk4.c
 * PURPOSE: Check module pagination, stale connection handling and retained panel controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_modules.h"
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
    unsigned queries, destroyed;
    uint32_t last_first;
    uint64_t generation;
    bool denied, close_query, unknown, invalid;
} Host;
static UmiStatus Query(void *data, const UmiDebugModuleSession *expected, uint32_t first,
                       uint32_t count, UmiDebugModulePage *out)
{
    Host *host = data;
    ++host->queries;
    host->last_first = first;
    if (host->denied)
        return UMI_STATUS_PERMISSION_DENIED;
    if (expected != NULL && expected->generation != host->generation)
        return UMI_STATUS_INVALID_STATE;
    memset(out, 0, sizeof *out);
    strcpy(out->session.session_id, "session");
    out->session.generation = host->generation;
    out->session.supported = 1;
    out->first = host->invalid ? first + 1U : first;
    out->requested_count = count;
    out->total_known = !host->unknown;
    out->total = 35U;
    out->count = first < 35U ? (size_t)(35U - first) : 0U;
    if (out->count > count)
        out->count = count;
    out->has_more = first + out->count < 35U;
    for (size_t i = 0U; i < out->count; ++i)
    {
        out->items[i].numeric_id = 1;
        out->items[i].number = (int64_t)((size_t)first + i);
        snprintf(out->items[i].name, sizeof out->items[i].name, "module-%zu", (size_t)first + i);
    }
    if (host->close_query)
        gtk_window_destroy(host->window);
    return UMI_STATUS_OK;
}
static void Destroy(gpointer data) { ++((Host *)data)->destroyed; }
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
    const char *mode = argv[1], *cases[] = {"paging", "unknown",  "denied",      "stale",
                                            "hidden", "retained", "close-query", "invalid-host"};
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
    host.generation = 1U;
    host.denied = strcmp(mode, "denied") == 0;
    host.unknown = strcmp(mode, "unknown") == 0;
    host.close_query = strcmp(mode, "close-query") == 0;
    host.invalid = strcmp(mode, "invalid-host") == 0;
    GtkWidget *panel = g_object_ref_sink(UmiGtk4DebugModulesCreate(Query, &host, Destroy));
    host.window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    gtk_window_set_child(host.window, panel);
    gtk_window_present(host.window);
    Drain();
    GtkWidget *refresh = umi_gtk4_automation_find_tagged_widget(panel, "debug.modules.refresh");
    GtkWidget *previous = umi_gtk4_automation_find_tagged_widget(panel, "debug.modules.previous");
    GtkWidget *next = umi_gtk4_automation_find_tagged_widget(panel, "debug.modules.next");
    CHECK(refresh != NULL && previous != NULL && next != NULL);
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(host.window);
    g_signal_emit_by_name(refresh, "clicked");
    if (strcmp(mode, "hidden") == 0 || strcmp(mode, "retained") == 0)
    {
        CHECK(host.queries == 0U);
        goto done;
    }
    CHECK(host.queries == 1U);
    if (host.denied || host.invalid)
    {
        CHECK(!gtk_widget_get_sensitive(next));
        goto done;
    }
    if (host.close_query)
    {
        CHECK(!gtk_widget_get_mapped(panel));
        goto done;
    }
    CHECK(gtk_widget_get_sensitive(next) && !gtk_widget_get_sensitive(previous));
    if (strcmp(mode, "stale") == 0)
        ++host.generation;
    g_signal_emit_by_name(next, "clicked");
    CHECK(host.queries == 2U);
    if (strcmp(mode, "stale") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(next) && !gtk_widget_get_sensitive(previous));
        goto done;
    }
    CHECK(host.last_first == 32U && !gtk_widget_get_sensitive(next) &&
          gtk_widget_get_sensitive(previous));
    g_signal_emit_by_name(previous, "clicked");
    CHECK(host.queries == 3U && host.last_first == 0U);
done:
    gtk_window_destroy(host.window);
    g_object_unref(host.window);
    g_object_unref(panel);
    if (host.destroyed != 1U)
        failed = 1;
    return failed;
}

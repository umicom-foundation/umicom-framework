/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_exceptions/test_gtk4.c
 * PURPOSE: Check exception panel ownership, state revisions and honest adapter verification labels.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_exception_filters.h"
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
    /* Retain the former member declaration for review. Match the panel's
     * filter aggregate while keeping every existing host/lifetime scenario. */
#if 0
    UmiDebugExceptionSnapshot snapshot;
#endif
    UmiDebugExceptionFiltersSnapshot snapshot;
    GtkWindow *window;
    unsigned reads, applies, destroyed;
    bool deny, close_read, close_apply, invalid;
} Host;
/* Retain the superseded signature for review. The callback writes the complete
 * filter aggregate required by UmiGtk4ExceptionFiltersRead. */
#if 0
static UmiStatus Read(void *data, UmiDebugExceptionSnapshot *out)
#endif
static UmiStatus Read(void *data, UmiDebugExceptionFiltersSnapshot *out)
{
    Host *host = data;
    ++host->reads;
    *out = host->snapshot;
    if (host->invalid)
        out->catalog.count = UMI_DEBUG_EXCEPTION_FILTER_LIMIT + 1U;
    if (host->close_read)
        gtk_window_destroy(host->window);
    return UMI_STATUS_OK;
}
static UmiStatus Apply(void *data, const UmiDebugExceptionSelection *selection)
{
    Host *host = data;
    ++host->applies;
    if (host->deny)
        return UMI_STATUS_PERMISSION_DENIED;
    if (selection->revision != host->snapshot.selection.revision)
        return UMI_STATUS_INVALID_STATE;
    host->snapshot.selection = *selection;
    ++host->snapshot.selection.revision;
    host->snapshot.acknowledged = 1;
    host->snapshot.acknowledgement.count = selection->enabled[0] ? 1U : 0U;
    if (host->close_apply)
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
    const char *mode = argv[1];
    const char *cases[] = {"normal", "unverified", "empty",      "denied",      "stale",
                           "hidden", "retained",   "close-read", "close-apply", "invalid-host"};
    bool known = false;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    Host *host = g_new0(Host, 1);
    host->snapshot.catalog.count = host->snapshot.selection.count =
        strcmp(mode, "empty") == 0 ? 0U : 1U;
    strcpy(host->snapshot.catalog.items[0].id, "adapter.filter");
    strcpy(host->snapshot.catalog.items[0].label, "Pause here");
    strcpy(host->snapshot.selection.session_id, "session");
    host->snapshot.selection.revision = 1U;
    host->snapshot.acknowledgement.verified[0] = strcmp(mode, "unverified") == 0 ? 0 : 1;
    host->deny = strcmp(mode, "denied") == 0;
    host->invalid = strcmp(mode, "invalid-host") == 0;
    host->close_read = strcmp(mode, "close-read") == 0;
    host->close_apply = strcmp(mode, "close-apply") == 0;
    GtkWidget *panel =
        g_object_ref_sink(UmiGtk4DebugExceptionFiltersCreate(Read, Apply, host, Destroy));
    host->window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    gtk_window_set_child(host->window, panel);
    gtk_window_present(host->window);
    Drain();
    GtkWidget *refresh = umi_gtk4_automation_find_tagged_widget(panel, "debug.exceptions.refresh");
    GtkWidget *apply = umi_gtk4_automation_find_tagged_widget(panel, "debug.exceptions.apply");
    CHECK(refresh != NULL && apply != NULL);
    g_signal_emit_by_name(refresh, "clicked");
    CHECK(host->reads == 1U);
    if (host->invalid || strcmp(mode, "empty") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(apply));
        goto done;
    }
    if (host->close_read)
    {
        CHECK(!gtk_widget_get_mapped(panel));
        goto done;
    }
    GtkWidget *check = umi_gtk4_automation_find_tagged_widget(panel, "debug.exceptions.filter.0");
    CHECK(check != NULL);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(check), TRUE);
    if (strcmp(mode, "stale") == 0)
        ++host->snapshot.selection.revision;
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(host->window);
    g_signal_emit_by_name(apply, "clicked");
    if (strcmp(mode, "hidden") == 0 || strcmp(mode, "retained") == 0)
        CHECK(host->applies == 0U);
    else
    {
        CHECK(host->applies == 1U);
        CHECK(host->snapshot.selection.enabled[0] == (!host->deny && strcmp(mode, "stale") != 0));
        CHECK(host->reads == 2U);
        if (strcmp(mode, "unverified") == 0)
        {
            /* Labels use plain text, never markup supplied by an adapter. */
            bool found = false;
            GtkWidget *rows = gtk_widget_get_last_child(panel);
            for (GtkWidget *child = gtk_widget_get_first_child(rows); child != NULL;
                 child = gtk_widget_get_next_sibling(child))
                if (GTK_IS_LABEL(child) &&
                    strstr(gtk_label_get_text(GTK_LABEL(child)), "Not verified") != NULL)
                    found = true;
            CHECK(found);
        }
    }
done:
    gtk_window_destroy(host->window);
    g_object_unref(host->window);
    g_object_unref(panel);
    if (host->destroyed != 1U)
        failed = 1;
    g_free(host);
    return failed;
}

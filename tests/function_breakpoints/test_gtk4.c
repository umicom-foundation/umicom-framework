/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/function_breakpoints/test_gtk4.c
 * PURPOSE: Exercise local draft editing, explicit Apply and retained function-breakpoint controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_function_breakpoints.h"
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
    UmiDebugFunctionSnapshot snapshot;
    GtkWindow *window;
    unsigned reads, applies, destroyed;
    bool denied, close_apply;
} Host;
static UmiStatus Read(void *data, UmiDebugFunctionSnapshot *out)
{
    Host *host = data;
    ++host->reads;
    *out = host->snapshot;
    return UMI_STATUS_OK;
}
static UmiStatus Apply(void *data, const UmiDebugFunctionDraft *draft)
{
    Host *host = data;
    ++host->applies;
    if (host->denied)
        return UMI_STATUS_PERMISSION_DENIED;
    if (draft->revision != host->snapshot.draft.revision)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UmiDebugFunctionDraftValidate(draft);
    if (status != UMI_STATUS_OK)
        return status;
    host->snapshot.draft = *draft;
    ++host->snapshot.draft.revision;
    host->snapshot.reply.count = 0U;
    host->snapshot.acknowledged = 1;
    for (size_t i = 0U; i < draft->count; ++i)
        if (draft->entries[i].enabled)
            host->snapshot.reply.entries[host->snapshot.reply.count++].verified = 1;
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
static GtkWidget *Find(GtkWidget *panel, const char *tag)
{
    return umi_gtk4_automation_find_tagged_widget(panel, tag);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"normal",
                           "empty-name",
                           "duplicate",
                           "remove",
                           "disable",
                           "condition",
                           "denied",
                           "stale",
                           "hidden",
                           "retained",
                           "close-apply",
                           "unsupported",
                           "editing-verification"};
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
    host->snapshot.supported = strcmp(mode, "unsupported") != 0;
    host->snapshot.conditions_supported = host->snapshot.hit_conditions_supported = 1;
    strcpy(host->snapshot.draft.session_id, "session");
    host->snapshot.draft.revision = 1U;
    host->denied = strcmp(mode, "denied") == 0;
    host->close_apply = strcmp(mode, "close-apply") == 0;
    GtkWidget *panel =
        g_object_ref_sink(UmiGtk4DebugFunctionBreakpointsCreate(Read, Apply, host, Destroy));
    host->window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    gtk_window_set_child(host->window, panel);
    gtk_window_present(host->window);
    Drain();
    GtkWidget *refresh = Find(panel, "debug.functions.refresh"),
              *add = Find(panel, "debug.functions.add"),
              *apply = Find(panel, "debug.functions.apply");
    CHECK(refresh != NULL && add != NULL && apply != NULL);
    g_signal_emit_by_name(refresh, "clicked");
    if (strcmp(mode, "unsupported") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(add) && !gtk_widget_get_sensitive(apply));
        goto done;
    }
    g_signal_emit_by_name(add, "clicked");
    GtkWidget *name = Find(panel, "debug.functions.0.name");
    CHECK(name != NULL);
    if (strcmp(mode, "empty-name") != 0)
        gtk_editable_set_text(GTK_EDITABLE(name), "main");
    if (strcmp(mode, "duplicate") == 0)
    {
        g_signal_emit_by_name(add, "clicked");
        name = Find(panel, "debug.functions.1.name");
        CHECK(name != NULL);
        gtk_editable_set_text(GTK_EDITABLE(name), "main");
    }
    if (strcmp(mode, "remove") == 0)
    {
        GtkWidget *remove = Find(panel, "debug.functions.0.remove");
        CHECK(remove != NULL);
        g_signal_emit_by_name(remove, "clicked");
    }
    if (strcmp(mode, "disable") == 0)
    {
        GtkWidget *enabled = Find(panel, "debug.functions.0.enabled");
        CHECK(enabled != NULL);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(enabled), FALSE);
    }
    if (strcmp(mode, "condition") == 0)
    {
        GtkWidget *condition = Find(panel, "debug.functions.0.condition");
        CHECK(condition != NULL);
        gtk_editable_set_text(GTK_EDITABLE(condition), "count > 5");
    }
    CHECK(host->applies == 0U);
    if (strcmp(mode, "stale") == 0)
        ++host->snapshot.draft.revision;
    if (strcmp(mode, "hidden") == 0)
        gtk_widget_set_visible(panel, FALSE);
    if (strcmp(mode, "retained") == 0)
        gtk_window_destroy(host->window);
    g_signal_emit_by_name(apply, "clicked");
    bool blocked = strcmp(mode, "empty-name") == 0 || strcmp(mode, "duplicate") == 0 ||
                   strcmp(mode, "hidden") == 0 || strcmp(mode, "retained") == 0;
    CHECK(host->applies == (blocked ? 0U : 1U));
    if (!blocked && !host->denied && strcmp(mode, "stale") != 0)
    {
        CHECK(host->snapshot.draft.count == (strcmp(mode, "remove") == 0 ? 0U : 1U));
        if (strcmp(mode, "disable") == 0)
            CHECK(host->snapshot.reply.count == 0U);
        if (strcmp(mode, "condition") == 0)
            CHECK(strcmp(host->snapshot.draft.entries[0].breakpoint.condition, "count > 5") == 0);
        if (strcmp(mode, "editing-verification") == 0)
        {
            name = Find(panel, "debug.functions.0.name");
            CHECK(name != NULL);
            gtk_editable_set_text(GTK_EDITABLE(name), "other");
            GtkWidget *rows = gtk_widget_get_last_child(panel),
                      *row = gtk_widget_get_first_child(rows);
            GtkWidget *verification = gtk_widget_get_last_child(row);
            CHECK(GTK_IS_LABEL(verification) &&
                  strstr(gtk_label_get_text(GTK_LABEL(verification)), "Unapplied edit") != NULL);
            CHECK(strcmp(host->snapshot.draft.entries[0].breakpoint.name, "main") == 0);
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

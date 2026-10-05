/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_quick_open_gtk4.c
 * PURPOSE: Exercise the native indexed-file picker with independent deterministic host callbacks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/quick_open.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
typedef struct Host
{
    GtkWindow *dialog;
    GtkWidget *query;
    uint64_t revision;
    unsigned reads, opens, destroyed;
    int closeRead, changeRead, badCount, badRoot, failRead, failOpen, literal;
    char opened[UMI_PATH_CAPACITY];
} Host;
static void Destroy(gpointer data) { ++((Host *)data)->destroyed; }
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *name = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (name != NULL && strcmp(name, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static void Pump(void)
{
    for (unsigned i = 0U; i < 512U && g_main_context_pending(NULL); ++i)
        (void)g_main_context_iteration(NULL, FALSE);
}
static size_t Rows(GtkWidget *list)
{
    size_t count = 0U;
    for (GtkWidget *row = gtk_widget_get_first_child(list); row != NULL;
         row = gtk_widget_get_next_sibling(row))
        if (GTK_IS_LIST_BOX_ROW(row))
            ++count;
    return count;
}
static UmiStatus Read(gpointer data, const char *query, size_t offset, uint64_t revision,
                      UmiFileIndexEntry *entries, size_t capacity, UmiFileIndexPage *out)
{
    Host *host = data;
    ++host->reads;
    if (host->closeRead)
    {
        host->closeRead = 0;
        gtk_window_destroy(host->dialog);
    }
    if (host->changeRead)
    {
        host->changeRead = 0;
        gtk_editable_set_text(GTK_EDITABLE(host->query), "file-069");
    }
    if (host->failRead)
        return UMI_STATUS_IO_ERROR;
    if (revision != 0U && revision != host->revision)
        return UMI_STATUS_BUSY;
    UmiFileIndexPage page = {0};
    page.stats.revision = host->revision;
    page.stats.files = 70U;
    page.offset = offset;
    (void)snprintf(page.stats.root, sizeof(page.stats.root), "C:/quick-open");
    for (size_t i = 0U; i < 70U; ++i)
    {
        char filename[64];
        if (host->literal && i == 0U)
            (void)snprintf(filename, sizeof(filename), "caf\xc3\xa9 <b>.c");
        else
            (void)snprintf(filename, sizeof(filename), "file-%03zu.c", i);
        if (strstr(filename, query) == NULL)
            continue;
        if (page.matched >= offset && page.count < capacity)
        {
            UmiFileIndexEntry *entry = &entries[page.count++];
            memset(entry, 0, sizeof(*entry));
            (void)snprintf(entry->path, sizeof(entry->path), "C:/quick-open/%s", filename);
            (void)snprintf(entry->relative_path, sizeof(entry->relative_path), "%s", filename);
            (void)snprintf(entry->name, sizeof(entry->name), "%s", filename);
        }
        ++page.matched;
    }
    page.has_more = offset < page.matched && page.count < page.matched - offset;
    if (host->badCount)
        page.count = capacity + 1U;
    if (host->badRoot)
        memset(page.stats.root, 'x', sizeof(page.stats.root));
    *out = page;
    return UMI_STATUS_OK;
}
static UmiStatus Open(gpointer data, const char *query, const UmiFileIndexPage *page, size_t position,
                      const UmiFileIndexEntry *entry)
{
    Host *host = data;
    (void)query;
    if (host->failOpen || page->stats.revision != host->revision)
        return UMI_STATUS_BUSY;
    if (position >= page->count)
        return UMI_STATUS_INVALID_ARGUMENT;
    ++host->opens;
    (void)snprintf(host->opened, sizeof(host->opened), "%s", entry->path);
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1], *cases[] = {"initial",
                                            "filter",
                                            "empty",
                                            "next",
                                            "previous",
                                            "stale-page",
                                            "refresh",
                                            "stale-open",
                                            "explicit-open",
                                            "query-invalidates",
                                            "retained-row",
                                            "retained-entry",
                                            "parent-close",
                                            "close-during-read",
                                            "changed-during-read",
                                            "coalesced",
                                            "malformed-count",
                                            "malformed-root",
                                            "read-failure",
                                            "literal-label",
                                            "escape",
                                            "destroy-once",
                                            "invalid-create"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    Host host = {0};
    host.revision = 1U;
    GtkWindow *parent = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
    GtkWidget *retained = NULL;
    GListModel *controllers = NULL;
    if (strcmp(name, "invalid-create") == 0)
    {
        GtkWindow *invalid = parent;
        CHECK(UmiGtk4QuickOpenCreate(NULL, Read, Open, &host, Destroy, &invalid) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              invalid == NULL);
        CHECK(UmiGtk4QuickOpenCreate(parent, NULL, Open, &host, Destroy, &invalid) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4QuickOpenCreate(parent, Read, NULL, &host, Destroy, &invalid) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiGtk4QuickOpenCreate(parent, Read, Open, &host, Destroy, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(host.destroyed == 0U);
        goto cleanup;
    }
    CHECK(UmiGtk4QuickOpenCreate(parent, Read, Open, &host, Destroy, &host.dialog) == UMI_STATUS_OK);
    host.query = Find(GTK_WIDGET(host.dialog), "umicom-quick-open-query");
    GtkWidget *list = Find(GTK_WIDGET(host.dialog), "umicom-quick-open-results"),
              *open = Find(GTK_WIDGET(host.dialog), "umicom-quick-open-open"),
              *next = Find(GTK_WIDGET(host.dialog), "umicom-quick-open-next"),
              *previous = Find(GTK_WIDGET(host.dialog), "umicom-quick-open-previous"),
              *refresh = Find(GTK_WIDGET(host.dialog), "umicom-quick-open-refresh"),
              *status = Find(GTK_WIDGET(host.dialog), "umicom-quick-open-status");
    CHECK(host.query != NULL && list != NULL && open != NULL && next != NULL && previous != NULL &&
          refresh != NULL && status != NULL);
    host.badCount = strcmp(name, "malformed-count") == 0;
    host.badRoot = strcmp(name, "malformed-root") == 0;
    host.failRead = strcmp(name, "read-failure") == 0;
    host.closeRead = strcmp(name, "close-during-read") == 0;
    host.changeRead = strcmp(name, "changed-during-read") == 0;
    host.literal = strcmp(name, "literal-label") == 0;
    Pump();
    if (host.badCount || host.badRoot || host.failRead)
    {
        CHECK(Rows(list) == 0U && !gtk_widget_get_sensitive(open) && host.opens == 0U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(status)), "not ready") != NULL);
        goto cleanup;
    }
    if (strcmp(name, "close-during-read") == 0)
    {
        CHECK(!UmiGtk4WindowIsOpen(host.dialog) && host.opens == 0U);
        goto cleanup;
    }
    if (strcmp(name, "changed-during-read") == 0)
    {
        CHECK(host.reads == 2U && Rows(list) == 1U);
        g_signal_emit_by_name(open, "clicked");
        CHECK(strstr(host.opened, "file-069.c") != NULL);
        goto cleanup;
    }
    CHECK(host.reads == 1U && Rows(list) == 64U && host.opens == 0U);
    if (strcmp(name, "initial") == 0)
    {
        CHECK(gtk_widget_get_sensitive(open) && gtk_widget_get_sensitive(next) &&
              !gtk_widget_get_sensitive(previous));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(status)), "of 70") != NULL);
    }
    else if (strcmp(name, "filter") == 0 || strcmp(name, "empty") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(host.query), strcmp(name, "empty") == 0 ? "missing" : "file-069");
        Pump();
        CHECK(Rows(list) == (strcmp(name, "empty") == 0 ? 0U : 1U));
        CHECK(host.opens == 0U);
    }
    else if (strcmp(name, "next") == 0 || strcmp(name, "previous") == 0)
    {
        g_signal_emit_by_name(next, "clicked");
        Pump();
        CHECK(Rows(list) == 6U && gtk_widget_get_sensitive(previous) && !gtk_widget_get_sensitive(next));
        if (strcmp(name, "previous") == 0)
        {
            g_signal_emit_by_name(previous, "clicked");
            Pump();
            CHECK(Rows(list) == 64U && !gtk_widget_get_sensitive(previous));
        }
        else
        {
            g_signal_emit_by_name(open, "clicked");
            CHECK(strstr(host.opened, "file-064.c") != NULL);
        }
    }
    else if (strcmp(name, "stale-page") == 0 || strcmp(name, "refresh") == 0)
    {
        ++host.revision;
        g_signal_emit_by_name(next, "clicked");
        Pump();
        CHECK(!gtk_widget_get_sensitive(open) && host.opens == 0U);
        if (strcmp(name, "refresh") == 0)
        {
            g_signal_emit_by_name(refresh, "clicked");
            Pump();
            CHECK(Rows(list) == 64U);
            g_signal_emit_by_name(open, "clicked");
            CHECK(host.opens == 1U);
        }
    }
    else if (strcmp(name, "stale-open") == 0)
    {
        ++host.revision;
        g_signal_emit_by_name(open, "clicked");
        CHECK(host.opens == 0U && UmiGtk4WindowIsOpen(host.dialog) && !gtk_widget_get_sensitive(open));
    }
    else if (strcmp(name, "explicit-open") == 0)
    {
        g_signal_emit_by_name(host.query, "activate");
        CHECK(host.opens == 1U && !UmiGtk4WindowIsOpen(host.dialog));
        g_signal_emit_by_name(open, "clicked");
        CHECK(host.opens == 1U);
    }
    else if (strcmp(name, "query-invalidates") == 0 || strcmp(name, "retained-row") == 0)
    {
        retained = g_object_ref(gtk_widget_get_first_child(list));
        gtk_editable_set_text(GTK_EDITABLE(host.query), "file-069");
        if (strcmp(name, "retained-row") == 0)
            Pump();
        g_signal_emit_by_name(list, "row-activated", retained);
        CHECK(host.opens == 0U);
        Pump();
        CHECK(Rows(list) == 1U);
    }
    else if (strcmp(name, "retained-entry") == 0 || strcmp(name, "parent-close") == 0)
    {
        retained = g_object_ref(host.query);
        if (strcmp(name, "parent-close") == 0)
            gtk_window_destroy(parent);
        else
            gtk_window_destroy(host.dialog);
        gtk_editable_set_text(GTK_EDITABLE(retained), "file-069");
        Pump();
        CHECK(host.reads == 1U && host.opens == 0U && !UmiGtk4WindowIsOpen(host.dialog));
    }
    else if (strcmp(name, "coalesced") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(host.query), "file-010");
        gtk_editable_set_text(GTK_EDITABLE(host.query), "file-069");
        Pump();
        CHECK(host.reads == 2U && Rows(list) == 1U && host.opens == 0U);
    }
    else if (strcmp(name, "literal-label") == 0)
    {
        GtkWidget *row = gtk_widget_get_first_child(list),
                  *label = gtk_list_box_row_get_child(GTK_LIST_BOX_ROW(row));
        CHECK(strcmp(gtk_label_get_text(GTK_LABEL(label)), "caf\xc3\xa9 <b>.c") == 0 &&
              !gtk_label_get_use_markup(GTK_LABEL(label)));
    }
    else if (strcmp(name, "escape") == 0)
    {
        controllers = gtk_widget_observe_controllers(GTK_WIDGET(host.dialog));
        int found = 0;
        for (guint i = 0U; i < g_list_model_get_n_items(controllers); ++i)
        {
            GtkEventController *controller = g_list_model_get_item(controllers, i);
            const char *id = gtk_event_controller_get_name(controller);
            if (id != NULL && strcmp(id, "umicom-quick-open-keys") == 0)
            {
                gboolean handled = FALSE;
                g_signal_emit_by_name(controller, "key-pressed", GDK_KEY_Escape, 0U, (GdkModifierType)0,
                                      &handled);
                found = handled;
            }
            g_object_unref(controller);
        }
        CHECK(found && !UmiGtk4WindowIsOpen(host.dialog));
    }
    else if (strcmp(name, "destroy-once") == 0)
    {
        gtk_window_destroy(host.dialog);
        g_clear_object(&host.dialog);
        CHECK(host.destroyed == 1U);
    }
cleanup:
    g_clear_object(&controllers);
    g_clear_object(&retained);
    if (host.dialog != NULL)
    {
        gtk_window_destroy(host.dialog);
        g_clear_object(&host.dialog);
    }
    gtk_window_destroy(parent);
    g_object_unref(parent);
    return failed;
}

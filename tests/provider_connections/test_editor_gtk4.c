/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_editor_gtk4.c
 * PURPOSE: Exercise real GTK draft, conflict and worker-retirement paths in isolated storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/provider_connections_internal.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>

/* Private state is inspected only to hold a lifetime reference while testing
 * destruction during a job. Actions use the real tagged GTK controls and the
 * actual SQLite service. No personal settings, vault or network is accessed. */
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "line %d: %s\n", __LINE__, #value); return 1; } } while (0)
#define OK(value) CHECK((value) == UMI_STATUS_OK)
typedef struct Fixture {
    UmiProviderConnectionsGtk *panel;
    UmiDataServer *server;
    UmiProviderConnections *store;
    UmiProviderConnectionSnapshot snapshot;
    char *path;
} Fixture;
static Fixture f;
static bool Wait(UmiProviderConnectionsGtk *panel)
{
    gint64 deadline = g_get_monotonic_time() + 8000000;
    while (UmiProviderConnectionsGtkBusy(panel) && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return !UmiProviderConnectionsGtkBusy(panel);
}
static bool Retire(void)
{
    if (f.panel == NULL) return true;
    UmiProviderConnectionsGtk *panel = f.panel;
    ++panel->references;
    UmiProviderConnectionsGtkDestroy(panel);
    gint64 deadline = g_get_monotonic_time() + 8000000;
    while (panel->references > 1U && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    bool finished = panel->references == 1U;
    UmiProviderEditorRelease(panel); f.panel = NULL;
    return finished;
}
static GtkWidget *Find(GtkWidget *widget, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(widget), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0) return widget;
    for (GtkWidget *child = gtk_widget_get_first_child(widget); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id);
        if (found != NULL) return found;
    }
    return NULL;
}
static GtkWidget *Control(const char *id)
{
    return Find(UmiProviderConnectionsGtkWidget(f.panel), id);
}
static void Click(const char *id)
{
    GtkWidget *button = Control(id);
    if (button != NULL) g_signal_emit_by_name(button, "clicked");
}
static void Text(const char *id, const char *value)
{
    gtk_editable_set_text(GTK_EDITABLE(Control(id)), value);
}
static void Check(const char *id, bool value)
{
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(id)), value);
}
static int Open(void)
{
    UmiProviderConnectionsGtkConfig config = {"studio", "desktop", f.path};
    OK(UmiProviderConnectionsGtkCreate(&config, &f.panel));
    return 0;
}
static int Seed(void)
{
    Text("connections.id", "local-model");
    Text("connections.label", "My local model");
    Click("connections.save"); CHECK(Wait(f.panel));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 1U && f.snapshot.revision == 1U);
    return 0;
}
static int SaveReopen(void)
{
    CHECK(Seed() == 0);
    CHECK(!f.panel->dirty && f.panel->editing_existing);
    CHECK(Retire()); CHECK(Open() == 0); CHECK(Wait(f.panel));
    GtkDropDown *selector = GTK_DROP_DOWN(Control("connections.select"));
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(selector)) == 1U);
    gtk_drop_down_set_selected(selector, 0U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Control("connections.label"))), "My local model") == 0);
    CHECK(!gtk_editable_get_editable(GTK_EDITABLE(Control("connections.id"))));
    return 0;
}
static int Conflict(void)
{
    CHECK(Seed() == 0);
    Text("connections.label", "This window's draft");
    strcpy(f.snapshot.items[0].label, "Other window");
    uint64_t revision = 0U;
    OK(UmiProviderConnectionsPut(f.store, &f.snapshot.items[0], true, 1U, &revision));
    Click("connections.save"); CHECK(Wait(f.panel));
    CHECK(f.panel->dirty && !f.panel->loaded && f.panel->draft_revision == 1U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Control("connections.label"))), "This window's draft") == 0);
    Click("connections.reload"); CHECK(Wait(f.panel));
    CHECK(f.panel->loaded && f.panel->snapshot.revision == 2U && f.panel->draft_revision == 1U);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Control("connections.saved"))), "Other window") != NULL);
    Click("connections.rebase"); CHECK(f.panel->draft_revision == 1U);
    Check("connections.confirm-review", true); Click("connections.rebase");
    CHECK(f.panel->draft_revision == 2U && f.panel->dirty);
    Click("connections.save"); CHECK(Wait(f.panel));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.revision == 3U && strcmp(f.snapshot.items[0].label, "This window's draft") == 0);
    return 0;
}
static int Selection(void)
{
    CHECK(Seed() == 0);
    UmiProviderConnection other = f.snapshot.items[0]; strcpy(other.id, "second"); strcpy(other.label, "Second connection");
    uint64_t revision = 0U;
    OK(UmiProviderConnectionsPut(f.store, &other, false, 1U, &revision));
    Click("connections.reload"); CHECK(Wait(f.panel));
    Text("connections.label", "Keep this draft");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Control("connections.select")), 1U);
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(Control("connections.select"))) == 0U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Control("connections.label"))), "Keep this draft") == 0);
    Check("connections.discard", true);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Control("connections.select")), 1U);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Control("connections.label"))), "Second connection") == 0);
    CHECK(!f.panel->dirty && !gtk_check_button_get_active(GTK_CHECK_BUTTON(Control("connections.discard"))));
    Text("connections.label", "Another draft"); Click("connections.new");
    CHECK(f.panel->editing_existing);
    Check("connections.discard", true); Click("connections.new");
    CHECK(!f.panel->editing_existing && !f.panel->dirty);
    return 0;
}
static int Remove(void)
{
    CHECK(Seed() == 0);
    Click("connections.remove"); CHECK(!f.panel->busy);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot)); CHECK(f.snapshot.count == 1U);
    Text("connections.label", "Unsaved change");
    Check("connections.confirm-remove", true); Click("connections.remove"); CHECK(!f.panel->busy);
    Check("connections.discard", true); Click("connections.remove"); CHECK(Wait(f.panel));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 0U && f.snapshot.revision == 2U && !f.panel->editing_existing);
    return 0;
}
static int Validation(void)
{
    Text("connections.id", "local-model");
    Text("connections.reference", "vault:personal-key");
    Click("connections.save"); CHECK(!f.panel->busy && f.panel->dirty);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot)); CHECK(f.snapshot.count == 0U);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Control("connections.route")), 0U);
    Click("connections.save"); CHECK(!f.panel->busy);
    Text("connections.endpoint", "https://api.example.invalid/inference");
    Click("connections.save"); CHECK(Wait(f.panel));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 1U && strcmp(f.snapshot.items[0].secret_reference, "vault:personal-key") == 0);
    return 0;
}
static int LongLabel(void)
{
    Text("connections.id", "local-model");
    char label[163];
    for (size_t i = 0U; i < 81U; ++i) { label[i * 2U] = (char)0xC3; label[i * 2U + 1U] = (char)0xA9; }
    label[162] = '\0'; Text("connections.label", label);
    Click("connections.save"); CHECK(!f.panel->busy && f.panel->dirty);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Control("connections.label"))), label) == 0);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot)); CHECK(f.snapshot.count == 0U);
    return 0;
}
static int CloseGuard(void)
{
    CHECK(UmiProviderConnectionsGtkCanClose(f.panel));
    Text("connections.id", "local-model");
    CHECK(!UmiProviderConnectionsGtkCanClose(f.panel));
    Check("connections.discard", true); CHECK(UmiProviderConnectionsGtkCanClose(f.panel));
    Text("connections.label", "Changed again"); CHECK(!UmiProviderConnectionsGtkCanClose(f.panel));
    Click("connections.save"); CHECK(!UmiProviderConnectionsGtkCanClose(f.panel));
    CHECK(Wait(f.panel)); CHECK(UmiProviderConnectionsGtkCanClose(f.panel));
    return 0;
}
static int FailedSave(void)
{
    CHECK(Seed() == 0);
    OK(umi_data_server_execute(f.server,
        "CREATE TRIGGER reject_editor BEFORE INSERT ON umicom_kv "
        "WHEN NEW.key='provider.connections/studio/desktop/catalogue' "
        "BEGIN SELECT RAISE(ABORT,'fixture rejected'); END;"));
    Text("connections.label", "Keep failed draft");
    Click("connections.save"); CHECK(Wait(f.panel));
    CHECK(f.panel->dirty && !f.panel->loaded);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(Control("connections.label"))), "Keep failed draft") == 0);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.revision == 1U && strcmp(f.snapshot.items[0].label, "My local model") == 0);
    OK(umi_data_server_execute(f.server, "DROP TRIGGER reject_editor;"));
    Click("connections.reload"); CHECK(Wait(f.panel));
    Click("connections.save"); CHECK(Wait(f.panel));
    CHECK(!f.panel->dirty && f.panel->loaded);
    return 0;
}
static int BadLoad(void)
{
    OK(umi_data_server_set(f.server, "provider.connections/studio/desktop/catalogue", "damaged fixture"));
    Click("connections.reload"); CHECK(Wait(f.panel));
    CHECK(!f.panel->loaded && !gtk_widget_get_sensitive(Control("connections.save")));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(Control("connections.message"))), "damaged") != NULL);
    char value[80];
    OK(umi_data_server_get(f.server, "provider.connections/studio/desktop/catalogue", value, sizeof(value)));
    CHECK(strcmp(value, "damaged fixture") == 0);
    OK(umi_data_server_set(f.server, "provider.connections/studio/desktop/catalogue", "catalogue\n1\n0\n"));
    Click("connections.reload"); CHECK(Wait(f.panel)); CHECK(f.panel->loaded);
    return 0;
}
static int Retained(bool write)
{
    GtkWidget *root = g_object_ref(UmiProviderConnectionsGtkWidget(f.panel));
    GtkWidget *button = g_object_ref(Control("connections.save"));
    if (write) { Text("connections.id", "local-model"); Click("connections.save"); }
    else Click("connections.reload");
    bool finished = Retire();
    /* A retained root deliberately keeps signal connections alive. Retirement
     * must clear its controller binding, not rely on object finalization. */
    g_signal_emit_by_name(button, "clicked");
    bool retired = g_object_get_data(G_OBJECT(root), "umicom-provider-editor") == NULL && !gtk_widget_get_sensitive(root);
    g_object_unref(button); g_object_unref(root);
    CHECK(finished && retired);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == (write ? 1U : 0U));
    return 0;
}
static int Invalid(void)
{
    UmiProviderConnectionsGtk *panel = NULL;
    UmiProviderConnectionsGtkConfig config = {"studio", "desktop", "relative.sqlite3"};
    CHECK(UmiProviderConnectionsGtkCreate(&config, &panel) == UMI_STATUS_INVALID_ARGUMENT && panel == NULL);
    config.database_path = f.path; config.profile_id = "../owner";
    CHECK(UmiProviderConnectionsGtkCreate(&config, &panel) == UMI_STATUS_INVALID_ARGUMENT && panel == NULL);
    CHECK(UmiProviderConnectionsGtkCreate(NULL, &panel) == UMI_STATUS_INVALID_ARGUMENT && panel == NULL);
    return 0;
}
static int Run(const char *name)
{
    CHECK(Open() == 0); CHECK(Wait(f.panel)); CHECK(f.panel->loaded);
    const char *controls[] = {"connections.select", "connections.save", "connections.remove", "connections.reload",
        "connections.id", "connections.label", "connections.saved", "connections.rebase", "connections.message"};
    for (size_t i = 0U; i < sizeof(controls) / sizeof(controls[0]); ++i) CHECK(Control(controls[i]) != NULL);
    if (strcmp(name, "keys-entry") == 0) {
        CHECK(Control("connections.keys") != NULL);
        Click("connections.keys");
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(Control("connections.message"))), "application window") != NULL);
        CHECK(strcmp(f.panel->application, "studio") == 0 && strcmp(f.panel->profile, "desktop") == 0);
        CHECK(!f.panel->dirty && !f.panel->busy);
        return 0;
    }
    if (strcmp(name, "save-reopen") == 0) return SaveReopen();
    if (strcmp(name, "conflict") == 0) return Conflict();
    if (strcmp(name, "selection") == 0) return Selection();
    if (strcmp(name, "remove") == 0) return Remove();
    if (strcmp(name, "validation") == 0) return Validation();
    if (strcmp(name, "long-label") == 0) return LongLabel();
    if (strcmp(name, "close-guard") == 0) return CloseGuard();
    if (strcmp(name, "failed-save") == 0) return FailedSave();
    if (strcmp(name, "bad-load") == 0) return BadLoad();
    if (strcmp(name, "retained") == 0) return Retained(false);
    if (strcmp(name, "pending-save-close") == 0) return Retained(true);
    return 2;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    char *directory = g_get_current_dir();
    f.path = g_build_filename(directory, "editor.sqlite3", NULL); g_free(directory);
    (void)g_remove(f.path);
    UmiStatus status = umi_data_server_create_sqlite(f.path, &f.server);
    int result = status == UMI_STATUS_UNAVAILABLE ? 77 : status == UMI_STATUS_OK ? 0 : 1;
    if (result == 0) {
        status = UmiProviderConnectionsOpen(f.server, "studio", "desktop", &f.store);
        result = status == UMI_STATUS_OK ? (strcmp(argv[1], "invalid") == 0 ? Invalid() : Run(argv[1])) : 1;
    }
    bool finished = Retire();
    UmiProviderConnectionsDestroy(f.store);
    umi_data_server_destroy(f.server);
    if (finished) (void)g_remove(f.path);
    else result = 1;
    g_free(f.path);
    return result;
}

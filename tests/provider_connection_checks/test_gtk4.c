/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connection_checks/test_gtk4.c
 * PURPOSE: Exercise model-check approval, retirement and password clearing without contacting a provider.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/provider_connections_internal.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"line %d: %s\n",__LINE__,#c); exit(1); } } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)
static UmiProviderConnectionsGtk *panel;
static UmiProviderConnections *store;
static UmiDataServer *server;
static const char *scenario;
static gint calls;
static bool local;
static bool Wait(void)
{
    gint64 deadline = g_get_monotonic_time() + 8000000;
    while (panel->busy && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return !panel->busy;
}
/* Only the remote operation is injected. The panel, SQLite worker, selection,
 * password hand-off and cancellation owner are the production implementations.
 * No operating-system vault, user password or network service is accessed. */
static UmiStatus FakeRun(const UmiProviderConnectionCheckPlan *plan, UmiProviderConnections *settings,
    bool approved, const char *password, const UmiCancellationToken *token, UmiProviderConnectionCheckResult *out)
{
    g_atomic_int_inc(&calls);
    CHECK(approved && strcmp(plan->application,"studio") == 0 && strcmp(plan->profile,"desktop") == 0);
    CHECK(strcmp(plan->connection.id,"test") == 0 && strcmp(plan->connection.model,"test-model") == 0);
    CHECK(strcmp(password,local ? "" : "test-profile-password") == 0);
    CHECK(plan->requires_credential == !local);
    UmiProviderConnectionCheckPlan current = {0};
    UmiStatus status = UmiProviderConnectionCheckPrepare(settings,"studio","desktop","test",plan->revision,&current);
    if (status != UMI_STATUS_OK) return status;
    if (strcmp(scenario,"cancel") == 0 || strcmp(scenario,"retained") == 0) {
        gint64 deadline = g_get_monotonic_time() + 5000000;
        while (!umi_cancellation_token_is_requested(token) && g_get_monotonic_time() < deadline) g_usleep(1000U);
        CHECK(umi_cancellation_token_is_requested(token)); return UMI_STATUS_CANCELLED;
    }
    if (strcmp(scenario,"denied-retry") == 0) return UMI_STATUS_PERMISSION_DENIED;
    out->http_status = 200U; out->listed_models = 2U;
    out->model_listed = strcmp(scenario,"missing-model") != 0;
    return out->model_listed ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
static void Approve(void)
{
    gtk_editable_set_text(GTK_EDITABLE(panel->check_password),local ? "" : "test-profile-password");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->check_confirm),TRUE);
}
static void Click(GtkWidget *button) { g_signal_emit_by_name(button,"clicked"); }
static void Cleared(void)
{
    CHECK(gtk_editable_get_text(GTK_EDITABLE(panel->check_password))[0] == '\0');
    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->check_confirm)));
}
static void Retire(void)
{
    ++panel->references; UmiProviderConnectionsGtkDestroy(panel);
    gint64 deadline = g_get_monotonic_time() + 8000000;
    while (panel->references > 1U && g_get_monotonic_time() < deadline) {
        while (g_main_context_iteration(NULL,FALSE)) {}
        g_usleep(1000U);
    }
    CHECK(panel->references == 1U && panel->check_token == NULL);
    UmiProviderEditorRelease(panel); panel = NULL;
}
static void Run(void)
{
    CHECK(Wait() && panel->loaded);
    panel->check_run = FakeRun;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->selector),0U);
    UmiProviderCheckRefresh(panel);
    CHECK(panel->editing_existing && gtk_widget_get_sensitive(panel->check_start));
    CHECK(UmiGtk4RecordingIsPrivate(panel->check_password));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->check_detail)),"/v1/models") != NULL);
    CHECK(!gtk_widget_get_sensitive(panel->check_cancel));
    if (strcmp(scenario,"no-approval") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->check_password),"test-profile-password");
        Click(panel->check_start); Cleared(); CHECK(!panel->busy && g_atomic_int_get(&calls) == 0); return;
    }
    Approve();
    if (strcmp(scenario,"changed-password") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->check_password),"another-test-password");
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->check_confirm)));
        Click(panel->check_start); Cleared(); CHECK(g_atomic_int_get(&calls) == 0); return;
    }
    if (strcmp(scenario,"dirty") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(panel->label),"Unsaved label"); Cleared();
        CHECK(panel->dirty && !gtk_widget_get_sensitive(panel->check_start));
        Click(panel->check_start); CHECK(!panel->busy && g_atomic_int_get(&calls) == 0); return;
    }
    if (strcmp(scenario,"stale") == 0) {
        UmiProviderConnection changed = panel->snapshot.items[0]; strcpy(changed.label,"Another window");
        uint64_t revision = 0U; OK(UmiProviderConnectionsPut(store,&changed,true,1U,&revision));
    }
    Click(panel->check_start); CHECK(panel->busy && panel->check_token != NULL); Cleared();
    CHECK(!gtk_widget_get_sensitive(panel->form) && gtk_widget_get_sensitive(panel->check_cancel));
    CHECK(!UmiProviderConnectionsGtkCanClose(panel));
    if (strcmp(scenario,"retained") == 0) {
        GtkWidget *root = g_object_ref(panel->root), *button = g_object_ref(panel->check_start);
        GtkWidget *password = g_object_ref(panel->check_password);
        Retire();
        int before = g_atomic_int_get(&calls); Click(button);
        CHECK(g_atomic_int_get(&calls) == before);
        CHECK(g_object_get_data(G_OBJECT(root),"umicom-provider-editor") == NULL && !gtk_widget_get_sensitive(root));
        CHECK(gtk_editable_get_text(GTK_EDITABLE(password))[0] == '\0');
        g_object_unref(password); g_object_unref(button); g_object_unref(root); return;
    }
    if (strcmp(scenario,"cancel") == 0) Click(panel->check_cancel);
    CHECK(Wait()); Cleared(); CHECK(panel->check_token == NULL && gtk_widget_get_sensitive(panel->form));
    CHECK(!gtk_widget_get_sensitive(panel->check_cancel));
    const char *message = gtk_label_get_text(GTK_LABEL(panel->check_result));
    const char *expected = strcmp(scenario,"stale") == 0 ? "Saved settings changed" :
        strcmp(scenario,"cancel") == 0 ? "Check cancelled" : strcmp(scenario,"denied-retry") == 0 ? "not accepted" :
        strcmp(scenario,"missing-model") == 0 ? "not found" : "catalogue lists";
    CHECK(strstr(message,expected) != NULL && strstr(message,"saved revision 1") != NULL);
    CHECK(strstr(message,"test-profile-password") == NULL);
    if (strcmp(scenario,"cancel") != 0) CHECK(g_atomic_int_get(&calls) == 1);
    if (strcmp(scenario,"denied-retry") == 0) {
        /* Fix the per-panel deadline relative to this assertion so a heavily
         * loaded runner cannot turn the retry test into a timing accident. */
        CHECK(panel->check_retry_after > 0); panel->check_retry_after = g_get_monotonic_time() + 30000000;
        Approve(); Click(panel->check_start); Cleared(); CHECK(!panel->busy && g_atomic_int_get(&calls) == 1);
    }
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    scenario = argv[1];
    const char *cases[] = {"remote","local","no-approval","changed-password","dirty","stale","cancel","retained","denied-retry","missing-model"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases)/sizeof(cases[0]); ++i) if (strcmp(scenario,cases[i]) == 0) known = true;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    local = strcmp(scenario,"local") == 0;
    char *directory = g_get_current_dir(); char *path = g_build_filename(directory,"check.sqlite3",NULL); g_free(directory);
    (void)g_remove(path);
    UmiStatus status = umi_data_server_create_sqlite(path,&server);
    if (status == UMI_STATUS_UNAVAILABLE) { g_free(path); return 77; }
    OK(status); OK(UmiProviderConnectionsOpen(server,"studio","desktop",&store));
    UmiProviderConnection value = {0}; strcpy(value.id,"test"); strcpy(value.label,"Test connection"); strcpy(value.model,"test-model");
    strcpy(value.provider_id,local ? "local-chat" : "openai");
    strcpy(value.endpoint,local ? "http://127.0.0.1:8080/v1/chat/completions" : "https://api.openai.com/v1/responses");
    if (!local) strcpy(value.secret_reference,"vault:test");
    value.route = local ? UMI_PROVIDER_CONNECTION_LOOPBACK : UMI_PROVIDER_CONNECTION_HTTPS;
    value.enabled = true; value.timeout_ms = 30000U;
    uint64_t revision = 0U; OK(UmiProviderConnectionsPut(store,&value,false,0U,&revision));
    UmiProviderConnectionsGtkConfig config = {"studio","desktop",path}; OK(UmiProviderConnectionsGtkCreate(&config,&panel));
    Run(); if (panel != NULL) Retire();
    UmiProviderConnectionsDestroy(store); umi_data_server_destroy(server); (void)g_remove(path); g_free(path); return 0;
}

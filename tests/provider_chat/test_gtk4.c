/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_chat/test_gtk4.c
 * PURPOSE: Exercise native request review, private inputs and pending-worker retirement with isolated storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/provider_chat_internal.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
#define OK(c) CHECK((c) == UMI_STATUS_OK)
static UmiProviderChatGtk *panel;
static const char *scenario;
static gint calls;
static UmiStatus FakeRun(UmiProviderChatPlan *plan, UmiProviderConnections *store, bool approved,
                         const char *password, const UmiCancellationToken *token, UmiProviderChatResult *out)
{
    (void)store;
    g_atomic_int_inc(&calls);
    CHECK(approved && strcmp(password, "test-profile-password") == 0);
    CHECK(strstr(UmiProviderChatInput(plan), "Explain this") != NULL &&
          strstr(UmiProviderChatInput(plan), "Only this context") != NULL);
    if (strcmp(scenario, "cancel") == 0 || strcmp(scenario, "retained") == 0)
    {
        gint64 deadline = g_get_monotonic_time() + 5000000;
        while (!umi_cancellation_token_is_requested(token) && g_get_monotonic_time() < deadline)
            g_usleep(1000U);
        CHECK(umi_cancellation_token_is_requested(token));
        return UMI_STATUS_CANCELLED;
    }
    if (strcmp(scenario, "denied") == 0)
        return UMI_STATUS_PERMISSION_DENIED;
    out->http_status = 200U;
    strcpy(out->text, "Text only reply");
    strcpy(out->model, "test-model");
    return UMI_STATUS_OK;
}
static void Wait(void)
{
    gint64 deadline = g_get_monotonic_time() + 8000000;
    while (panel->busy && g_get_monotonic_time() < deadline)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
    CHECK(!panel->busy);
}
static void Retire(void)
{
    ++panel->references;
    UmiProviderChatGtkDestroy(panel);
    gint64 deadline = g_get_monotonic_time() + 8000000;
    while (panel->references > 1U && g_get_monotonic_time() < deadline)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        g_usleep(1000U);
    }
    CHECK(panel->references == 1U && panel->token == NULL);
    UmiProviderChatGtkRelease(panel);
    panel = NULL;
}
static void Click(GtkWidget *button) { g_signal_emit_by_name(button, "clicked"); }
static void Approve(void)
{
    gtk_editable_set_text(GTK_EDITABLE(panel->password), "test-profile-password");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approve), TRUE);
}
static void Cleared(void)
{
    CHECK(gtk_editable_get_text(GTK_EDITABLE(panel->password))[0] == '\0' &&
          !gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approve)));
}
static void Run(UmiProviderConnections *store, UmiProviderConnection *connection)
{
    panel->run = FakeRun;
    UmiProviderChatGtkText(panel->prompt, "Explain this");
    UmiProviderChatGtkText(panel->context, "Only this context");
    CHECK(!UmiProviderChatGtkCanClose(panel));
    CHECK(UmiGtk4RecordingIsPrivate(panel->prompt) && UmiGtk4RecordingIsPrivate(panel->context) &&
          UmiGtk4RecordingIsPrivate(panel->result) && UmiGtk4RecordingIsPrivate(panel->password));
    if (strcmp(scenario, "stale-review") == 0)
    {
        uint64_t revision;
        strcpy(connection->label, "Another writer");
        OK(UmiProviderConnectionsPut(store, connection, true, 1U, &revision));
    }
    Click(panel->review);
    CHECK(panel->busy);
    Wait();
    if (strcmp(scenario, "stale-review") == 0)
    {
        CHECK(panel->plan == NULL && g_atomic_int_get(&calls) == 0);
        return;
    }
    CHECK(panel->plan != NULL && gtk_widget_get_sensitive(panel->send));
    if (strcmp(scenario, "no-approval") == 0)
    {
        Click(panel->send);
        CHECK(!panel->busy && panel->plan != NULL && g_atomic_int_get(&calls) == 0);
        Cleared();
        return;
    }
    Approve();
    if (strcmp(scenario, "edit") == 0)
    {
        UmiProviderChatGtkText(panel->context, "Changed context");
        CHECK(panel->plan == NULL);
        Cleared();
        Click(panel->send);
        CHECK(g_atomic_int_get(&calls) == 0);
        return;
    }
    if (strcmp(scenario, "password-edit") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(panel->password), "new-test-password");
        CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approve)));
        Click(panel->send);
        Cleared();
        CHECK(g_atomic_int_get(&calls) == 0);
        return;
    }
    Click(panel->send);
    CHECK(panel->busy && panel->plan == NULL && panel->token != NULL);
    Cleared();
    CHECK(!UmiProviderChatGtkCanClose(panel));
    if (strcmp(scenario, "retained") == 0)
    {
        GtkWidget *root = g_object_ref(panel->root), *button = g_object_ref(panel->send);
        Retire();
        int before = g_atomic_int_get(&calls);
        Click(button);
        CHECK(g_atomic_int_get(&calls) == before &&
              g_object_get_data(G_OBJECT(root), "umicom-provider-chat") == NULL &&
              !gtk_widget_get_sensitive(root));
        g_object_unref(button);
        g_object_unref(root);
        return;
    }
    if (strcmp(scenario, "cancel") == 0)
        Click(panel->cancel);
    Wait();
    CHECK(panel->plan == NULL && !gtk_widget_get_sensitive(panel->send));
    Cleared();
    if (strcmp(scenario, "denied") == 0)
        CHECK(panel->retry_after > 0);
    if (strcmp(scenario, "send") == 0)
    {
        CHECK(g_atomic_int_get(&calls) == 1);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->result));
        CHECK(gtk_text_buffer_get_char_count(buffer) == 15);
    }
    Click(panel->send);
    CHECK(!panel->busy);
    Click(panel->clear);
    CHECK(!panel->dirty && UmiProviderChatGtkCanClose(panel));
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    scenario = argv[1];
    const char *cases[] = {"send",         "no-approval", "edit",     "password-edit",
                           "stale-review", "cancel",      "retained", "denied"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(scenario, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    char *directory = g_get_current_dir(), *path = g_build_filename(directory, "chat.sqlite3", NULL);
    g_free(directory);
    (void)g_remove(path);
    UmiDataServer *server = NULL;
    UmiProviderConnections *store = NULL;
    UmiStatus status = umi_data_server_create_sqlite(path, &server);
    if (status == UMI_STATUS_UNAVAILABLE)
    {
        g_free(path);
        return 77;
    }
    OK(status);
    OK(UmiProviderConnectionsOpen(server, "studio", "desktop", &store));
    UmiProviderConnection connection = {0};
    strcpy(connection.id, "test");
    strcpy(connection.label, "Chat");
    strcpy(connection.provider_id, "openai");
    strcpy(connection.model, "test-model");
    strcpy(connection.endpoint, "https://api.openai.com/v1/responses");
    strcpy(connection.secret_reference, "vault:test");
    connection.route = UMI_PROVIDER_CONNECTION_HTTPS;
    connection.enabled = true;
    connection.timeout_ms = 30000U;
    uint64_t revision;
    OK(UmiProviderConnectionsPut(store, &connection, false, 0U, &revision));
    UmiProviderChatGtkConfig config = {{"studio", "desktop", path}, "test", revision};
    OK(UmiProviderChatGtkCreate(&config, &panel));
    Run(store, &connection);
    if (panel != NULL)
        Retire();
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    (void)g_remove(path);
    g_free(path);
    return 0;
}

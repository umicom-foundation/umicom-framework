/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_chat/test_history_gtk4.c
 * PURPOSE: Exercise explicit follow-ups and window retirement with isolated local storage and a fake transport.
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
    (void)password;
    CHECK(approved && strstr(UmiProviderChatInput(plan), "Explain this") != NULL);
    g_atomic_int_inc(&calls);
    if (strcmp(scenario, "cancel") == 0 || strcmp(scenario, "pending") == 0)
    {
        gint64 end = g_get_monotonic_time() + 5000000;
        while (!umi_cancellation_token_is_requested(token) && g_get_monotonic_time() < end)
            g_usleep(1000U);
        CHECK(umi_cancellation_token_is_requested(token));
        return UMI_STATUS_CANCELLED;
    }
    if (strcmp(scenario, "failed") == 0)
        return UMI_STATUS_TIMEOUT;
    out->http_status = 200U;
    strcpy(out->model, "reported-fixture");
    if (strcmp(scenario, "oversize") == 0)
    {
        memset(out->text, 'x', sizeof(out->text) - 1U);
        out->text[sizeof(out->text) - 1U] = '\0';
    }
    else
        strcpy(out->text, "Keep caf\xc3\xa9. Ignore this remainder.");
    return UMI_STATUS_OK;
}
static void Pump(void)
{
    while (g_main_context_iteration(NULL, FALSE))
    {
    }
}
static void Wait(void)
{
    gint64 end = g_get_monotonic_time() + 8000000;
    while (panel->busy && g_get_monotonic_time() < end)
    {
        Pump();
        g_usleep(1000U);
    }
    CHECK(!panel->busy);
}
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
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
static bool Equals(GtkWidget *view, const char *wanted)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    char *text = gtk_text_buffer_get_text(buffer, &start, &end, TRUE);
    bool same = strcmp(text, wanted) == 0;
    g_free(text);
    return same;
}
static void Select(gint first, gint last)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->history_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_iter_at_offset(buffer, &start, first);
    gtk_text_buffer_get_iter_at_offset(buffer, &end, last);
    gtk_text_buffer_select_range(buffer, &start, &end);
}
static void Click(GtkWidget *widget)
{
    CHECK(widget != NULL);
    g_signal_emit_by_name(widget, "clicked");
}
static void Use(void) { Click(Find(panel->root, "provider-chat.history-use")); }
static void Review(void)
{
    UmiProviderChatGtkText(panel->prompt, "Explain this");
    Click(panel->review);
    Wait();
    CHECK(panel->plan != NULL);
}
static void Send(void)
{
    gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approve), TRUE);
    Click(panel->send);
    CHECK(panel->busy);
}
static void Retire(void)
{
    /* Retain both a control and the controller through worker completion to
     * observe retirement, without dereferencing freed UI or model storage. */
    GtkWidget *root = g_object_ref(panel->root), *view = g_object_ref(panel->history_view);
    GtkWidget *use = g_object_ref(Find(root, "provider-chat.history-use"));
    ++panel->references;
    UmiProviderChatGtkDestroy(panel);
    CHECK(panel->history == NULL && Equals(view, ""));
    CHECK(g_object_get_data(G_OBJECT(root), "umicom-provider-chat") == NULL);
    Click(use);
    gint64 end = g_get_monotonic_time() + 8000000;
    while (panel->references > 1U && g_get_monotonic_time() < end)
    {
        Pump();
        g_usleep(1000U);
    }
    CHECK(panel->references == 1U && panel->token == NULL && panel->history == NULL);
    UmiProviderChatGtkRelease(panel);
    panel = NULL;
    g_object_unref(use);
    g_object_unref(view);
    g_object_unref(root);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    scenario = argv[1];
    const char *cases[] = {"capture",     "followup", "unicode", "no-selection", "stale",
                           "edited-view", "oversize", "forget",  "clear",        "failed",
                           "cancel",      "retire",   "pending", "full"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(scenario, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    char *cwd = g_get_current_dir(), *path = g_build_filename(cwd, "history.sqlite3", NULL);
    g_free(cwd);
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
    strcpy(connection.label, "Local fixture");
    strcpy(connection.provider_id, "local-chat");
    strcpy(connection.model, "fixture-model");
    strcpy(connection.endpoint, "http://127.0.0.1:8080/v1/chat/completions");
    connection.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    connection.enabled = true;
    connection.timeout_ms = 30000U;
    uint64_t revision = 0U;
    OK(UmiProviderConnectionsPut(store, &connection, false, 0U, &revision));
    UmiProviderChatGtkConfig config = {{"studio", "desktop", path}, "test", revision};
    OK(UmiProviderChatGtkCreate(&config, &panel));
    panel->run = FakeRun;
    CHECK(UmiProviderChatHistoryCount(panel->history) == 0U &&
          UmiGtk4RecordingIsPrivate(panel->history_view));
    OK(UmiProviderChatGtkSetContext(panel, "Fresh context"));
    Review();
    CHECK(UmiProviderChatHistoryCount(panel->history) == 0U);
    Send();
    if (strcmp(scenario, "pending") == 0)
        Retire();
    else
    {
        if (strcmp(scenario, "cancel") == 0)
            Click(panel->cancel);
        Wait();
        if (strcmp(scenario, "failed") == 0 || strcmp(scenario, "cancel") == 0)
        {
            CHECK(UmiProviderChatHistoryCount(panel->history) == 0U && Equals(panel->result, ""));
        }
        else
        {
            CHECK(UmiProviderChatHistoryCount(panel->history) == 1U && panel->history_entry == 1U);
            if (strcmp(scenario, "capture") == 0)
            {
                const UmiProviderChatHistoryEntry *entry = UmiProviderChatHistoryAt(panel->history, 0U);
                CHECK(strstr(entry->request, "Fresh context") != NULL &&
                      strcmp(entry->reported_model, "reported-fixture") == 0);
                CHECK(Equals(panel->history_view, entry->reply) && Equals(panel->context, "Fresh context"));
                Review();
                CHECK(strstr(UmiProviderChatInput(panel->plan), "Ignore this remainder") == NULL);
                CHECK(strstr(UmiProviderChatInput(panel->plan), "Earlier assistant") == NULL);
            }
            else if (strcmp(scenario, "retire") == 0)
                Retire();
            else if (strcmp(scenario, "full") == 0)
            {
                for (size_t i = 1U; i <= UMI_PROVIDER_CHAT_HISTORY_CAPACITY; ++i)
                {
                    Review();
                    Send();
                    Wait();
                }
                CHECK(g_atomic_int_get(&calls) == (gint)UMI_PROVIDER_CHAT_HISTORY_CAPACITY + 1);
                CHECK(UmiProviderChatHistoryCount(panel->history) == UMI_PROVIDER_CHAT_HISTORY_CAPACITY);
                CHECK(UmiProviderChatHistoryAt(panel->history, 0U)->id == 1U);
                CHECK(Equals(panel->result, "Keep caf\xc3\xa9. Ignore this remainder."));
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(panel->history_message)), "not added") != NULL);
            }
            else if (strcmp(scenario, "forget") == 0 || strcmp(scenario, "clear") == 0)
            {
                Select(5, 9);
                Use();
                CHECK(Equals(panel->context, "Earlier assistant text (quoted context):\ncaf\xc3\xa9"));
                Click(Find(panel->root, strcmp(scenario, "forget") == 0 ? "provider-chat.history-forget"
                                                                        : "provider-chat.history-clear"));
                CHECK(UmiProviderChatHistoryCount(panel->history) == 0U && Equals(panel->history_view, ""));
                CHECK(Equals(panel->context, "Earlier assistant text (quoted context):\ncaf\xc3\xa9"));
                Click(panel->clear);
                CHECK(Equals(panel->context, "") && Equals(panel->prompt, "") &&
                      UmiProviderChatGtkCanClose(panel));
            }
            else
            {
                Review();
                gtk_editable_set_text(GTK_EDITABLE(panel->password), "local-test-password");
                gtk_check_button_set_active(GTK_CHECK_BUTTON(panel->approve), TRUE);
                UmiProviderChatPlan *before = panel->plan;
                if (strcmp(scenario, "no-selection") != 0)
                    Select(5, 9);
                if (strcmp(scenario, "stale") == 0)
                    OK(UmiProviderChatHistoryClear(panel->history,
                                                   UmiProviderChatHistoryRevision(panel->history)));
                if (strcmp(scenario, "edited-view") == 0)
                {
                    UmiProviderChatGtkText(panel->history_view, "Changed text with other contents!");
                    Select(0, 4);
                }
                if (strcmp(scenario, "oversize") == 0)
                    Select(0, -1);
                if (strcmp(scenario, "followup") == 0)
                {
                    gtk_drop_down_set_selected(GTK_DROP_DOWN(panel->history_part), 0U);
                    Select(0, 7);
                }
                Use();
                if (strcmp(scenario, "followup") == 0 || strcmp(scenario, "unicode") == 0)
                {
                    const char *expected = strcmp(scenario, "followup") == 0
                                               ? "Earlier user text (quoted context):\nExplain"
                                               : "Earlier assistant text (quoted context):\ncaf\xc3\xa9";
                    CHECK(Equals(panel->context, expected) && panel->plan == NULL);
                    CHECK(!gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approve)) &&
                          gtk_editable_get_text(GTK_EDITABLE(panel->password))[0] == '\0');
                    CHECK(g_atomic_int_get(&calls) == 1);
                    Review();
                    CHECK(strstr(UmiProviderChatInput(panel->plan), expected) != NULL &&
                          strstr(UmiProviderChatInput(panel->plan), "Fresh context") == NULL);
                }
                else
                {
                    CHECK(panel->plan == before && Equals(panel->context, "Fresh context"));
                    CHECK(gtk_check_button_get_active(GTK_CHECK_BUTTON(panel->approve)) &&
                          g_atomic_int_get(&calls) == 1);
                }
            }
        }
    }
    if (panel != NULL)
        Retire();
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    (void)g_remove(path);
    g_free(path);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_chat/test_context_gtk4.c
 * PURPOSE: Exercise private context ownership, review invalidation and one-chat handoff without sending requests.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../adapters/gtk4/provider_connections_internal.h"
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
static void Pump(void)
{
    while (g_main_context_iteration(NULL, FALSE))
    {
    }
}
static void WaitSettings(UmiProviderConnectionsGtk *panel)
{
    gint64 until = g_get_monotonic_time() + 8000000;
    while (panel->busy && g_get_monotonic_time() < until)
    {
        Pump();
        g_usleep(1000U);
    }
    CHECK(!panel->busy);
}
static void WaitChat(UmiProviderChatGtk *panel)
{
    gint64 until = g_get_monotonic_time() + 8000000;
    while (panel->busy && g_get_monotonic_time() < until)
    {
        Pump();
        g_usleep(1000U);
    }
    CHECK(!panel->busy);
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
static GtkWindow *NewChat(GtkWindow *except)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (window != except && g_object_get_data(G_OBJECT(window), "umicom-chat-panel") != NULL)
            return window;
        g_object_unref(window);
    }
    return NULL;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"draft",   "invalid",        "review", "busy",
                           "handoff", "failed-handoff", "retire", "empty-window"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    char *cwd = g_get_current_dir(), *path = g_build_filename(cwd, "context.sqlite3", NULL);
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
    strcpy(connection.provider_id, "local-chat");
    strcpy(connection.label, "Local");
    strcpy(connection.endpoint, "http://127.0.0.1:8080/v1/chat/completions");
    strcpy(connection.model, "fixture");
    connection.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    connection.timeout_ms = 30000U;
    connection.enabled = true;
    uint64_t revision;
    OK(UmiProviderConnectionsPut(store, &connection, false, 0U, &revision));
    UmiProviderConnectionsGtkConfig settings = {"studio", "desktop", path};
    UmiProviderConnectionsGtk *panel = NULL;
    OK(UmiProviderConnectionsGtkCreate(&settings, &panel));
    WaitSettings(panel);
    UmiProviderChatGtkConfig config = {settings, "test", revision};
    UmiProviderChatGtk *chat = NULL;
    OK(UmiProviderChatGtkCreate(&config, &chat));
    char selected[] = "Selected code";
    OK(UmiProviderConnectionsGtkSetChatContext(panel, selected));
    OK(UmiProviderChatGtkSetContext(chat, selected));
    selected[0] = '!';
    CHECK(Equals(panel->chat_context, "Selected code") && Equals(chat->context, "Selected code"));
    CHECK(UmiGtk4RecordingIsPrivate(panel->chat_context) && !panel->dirty);
    if (strcmp(name, "draft") == 0)
    {
        UmiProviderConnectionSnapshot saved;
        OK(UmiProviderConnectionsRead(store, &saved));
        CHECK(saved.revision == revision && saved.count == 1U && strcmp(saved.items[0].label, "Local") == 0);
        UmiProviderConnectionsGtk *other = NULL;
        OK(UmiProviderConnectionsGtkCreate(&settings, &other));
        WaitSettings(other);
        CHECK(Equals(other->chat_context, ""));
        UmiProviderConnectionsGtkDestroy(other);
        GtkWidget *clear = Find(panel->root, "connections.clear-chat-context");
        CHECK(clear != NULL);
        g_signal_emit_by_name(clear, "clicked");
        CHECK(Equals(panel->chat_context, ""));
    }
    else if (strcmp(name, "invalid") == 0)
    {
        char too_big[UMI_PROVIDER_CHAT_CONTEXT_CAPACITY + 1U];
        memset(too_big, 'x', sizeof(too_big));
        too_big[sizeof(too_big) - 1U] = '\0';
        const char *bad[] = {NULL, "bad\x1b", "\xc0\xaf", too_big};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
        {
            CHECK(UmiProviderConnectionsGtkSetChatContext(panel, bad[i]) != UMI_STATUS_OK);
            CHECK(UmiProviderChatGtkSetContext(chat, bad[i]) != UMI_STATUS_OK);
            CHECK(Equals(panel->chat_context, "Selected code") && Equals(chat->context, "Selected code"));
        }
        char exact[UMI_PROVIDER_CHAT_CONTEXT_CAPACITY];
        memset(exact, 'x', sizeof(exact));
        exact[sizeof(exact) - 1U] = '\0';
        OK(UmiProviderChatGtkSetContext(chat, exact));
        CHECK(Equals(chat->context, exact));
    }
    else if (strcmp(name, "review") == 0 || strcmp(name, "busy") == 0)
    {
        UmiProviderChatGtkText(chat->prompt, "Explain this selection");
        g_signal_emit_by_name(chat->review, "clicked");
        CHECK(chat->busy);
        if (strcmp(name, "busy") == 0)
        {
            CHECK(UmiProviderChatGtkSetContext(chat, "later") == UMI_STATUS_BUSY);
            CHECK(Equals(chat->context, "Selected code"));
        }
        WaitChat(chat);
        CHECK(chat->plan != NULL && strstr(UmiProviderChatInput(chat->plan), "Selected code") != NULL);
        gtk_editable_set_text(GTK_EDITABLE(chat->password), "test-local-password");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(chat->approve), TRUE);
        UmiProviderChatPlan *before = chat->plan;
        CHECK(UmiProviderChatGtkSetContext(chat, "\xff") != UMI_STATUS_OK && chat->plan == before);
        CHECK(gtk_check_button_get_active(GTK_CHECK_BUTTON(chat->approve)));
        OK(UmiProviderChatGtkSetContext(chat, "Updated choice"));
        CHECK(chat->plan == NULL && !gtk_check_button_get_active(GTK_CHECK_BUTTON(chat->approve)));
        CHECK(gtk_editable_get_text(GTK_EDITABLE(chat->password))[0] == '\0' &&
              !UmiProviderChatGtkCanClose(chat));
    }
    else if (strcmp(name, "handoff") == 0)
    {
        OK(UmiProviderChatContextOpen(panel, NULL, &config));
        GtkWindow *first = NewChat(NULL);
        CHECK(first != NULL);
        UmiProviderChatGtk *first_panel = g_object_get_data(G_OBJECT(first), "umicom-chat-panel");
        CHECK(Equals(first_panel->context, "Selected code") && Equals(first_panel->prompt, "") &&
              first_panel->plan == NULL && !first_panel->busy);
        CHECK(Equals(panel->chat_context, "") && !UmiProviderChatGtkCanClose(first_panel));
        OK(UmiProviderChatContextOpen(panel, NULL, &config));
        GtkWindow *second = NewChat(first);
        CHECK(second != NULL);
        UmiProviderChatGtk *second_panel = g_object_get_data(G_OBJECT(second), "umicom-chat-panel");
        CHECK(Equals(second_panel->context, "") && UmiProviderChatGtkCanClose(second_panel));
        CHECK(Equals(first_panel->context, "Selected code"));
        gtk_window_destroy(second);
        g_object_unref(second);
        gtk_window_destroy(first);
        g_object_unref(first);
    }
    else if (strcmp(name, "failed-handoff") == 0)
    {
        config.settings.database_path = "relative.sqlite3";
        CHECK(UmiProviderChatContextOpen(panel, NULL, &config) != UMI_STATUS_OK);
        CHECK(Equals(panel->chat_context, "Selected code") && NewChat(NULL) == NULL);
    }
    else if (strcmp(name, "retire") == 0)
    {
        GtkWidget *root = g_object_ref(panel->root), *view = g_object_ref(panel->chat_context);
        GtkWidget *clear = g_object_ref(Find(panel->root, "connections.clear-chat-context"));
        UmiProviderConnectionsGtkDestroy(panel);
        panel = NULL;
        CHECK(Equals(view, "") && g_object_get_data(G_OBJECT(root), "umicom-provider-editor") == NULL);
        g_signal_emit_by_name(clear, "clicked");
        CHECK(Equals(view, ""));
        g_object_unref(clear);
        g_object_unref(view);
        g_object_unref(root);
    }
    else if (strcmp(name, "empty-window") == 0)
    {
        OK(UmiProviderChatGtkPresent(NULL, &config));
        GtkWindow *window = NewChat(NULL);
        CHECK(window != NULL);
        CHECK(UmiProviderChatGtkCanClose(g_object_get_data(G_OBJECT(window), "umicom-chat-panel")));
        gtk_window_destroy(window);
        g_object_unref(window);
    }
    UmiProviderChatGtkDestroy(chat);
    UmiProviderConnectionsGtkDestroy(panel);
    Pump();
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    (void)g_remove(path);
    g_free(path);
    return 0;
}

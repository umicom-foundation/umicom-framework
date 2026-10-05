/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_chat/test_history.c
 * PURPOSE: Cover bounded ownership, stale selections and atomic excerpt composition without a provider.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../../src/provider_chat/history_internal.h"
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
static bool Zero(const void *data, size_t bytes)
{
    const unsigned char *cursor = data;
    for (size_t i = 0U; i < bytes; ++i)
        if (cursor[i] != 0U)
            return false;
    return true;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    const char *cases[] = {"lifecycle", "owned",  "large",         "full",  "invalid",   "stale",
                           "remove",    "clear",  "compose",       "order", "utf8",      "ranges",
                           "roles",     "atomic", "context-limit", "flags", "exhausted", "arguments"};
    bool known = false;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = true;
    if (!known)
        return 2;
    UmiDataServer *server = NULL;
    UmiProviderConnections *store = NULL;
    OK(umi_data_server_create_memory(&server));
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
    UmiProviderChatPlan *plan = NULL;
    OK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, "Explain the selection",
                              "Original context", 128U, &plan));
    UmiProviderChatHistory *history = NULL;
    UmiProviderChatResult *reply = calloc(1U, sizeof(*reply));
    CHECK(reply != NULL);
    reply->http_status = 200U;
    strcpy(reply->model, "reported-model");
    strcpy(reply->text, "Keep caf\xc3\xa9. Ignore this remainder.");
    OK(UmiProviderChatHistoryCreate(&history));
    CHECK(UmiProviderChatHistoryCount(history) == 0U && UmiProviderChatHistoryRevision(history) == 1U);
    uint64_t id = 0U;
    OK(UmiProviderChatHistoryAppend(history, plan, reply, &id));
    uint64_t current = UmiProviderChatHistoryRevision(history);
    CHECK(id == 1U && current == 2U);
    const UmiProviderChatHistoryEntry *entry = UmiProviderChatHistoryAt(history, 0U);
    CHECK(entry != NULL);
    char output[UMI_PROVIDER_CHAT_CONTEXT_CAPACITY];
    strcpy(output, "unchanged");
    UmiProviderChatHistoryExcerpt excerpt = {id, UMI_AI_ROLE_ASSISTANT, 5U, 5U};
    if (strcmp(name, "lifecycle") == 0)
    {
        CHECK(UmiProviderChatHistoryCount(history) == 1U && UmiProviderChatHistoryAt(history, 1U) == NULL);
        CHECK(strcmp(entry->request, UmiProviderChatInput(plan)) == 0 &&
              strcmp(entry->reply, reply->text) == 0);
        CHECK(strcmp(entry->application, "studio") == 0 && strcmp(entry->profile, "desktop") == 0);
        CHECK(strcmp(entry->connection, "test") == 0 && strcmp(entry->endpoint, connection.endpoint) == 0);
        CHECK(strcmp(entry->requested_model, "fixture-model") == 0 &&
              strcmp(entry->reported_model, "reported-model") == 0);
        CHECK(entry->connection_revision == revision && entry->output_limit == 128U && !entry->refused &&
              !entry->truncated);
    }
    else if (strcmp(name, "owned") == 0)
    {
        /* Retiring the request and changing the caller's result cannot alter a
         * complete exchange already owned by the history. */
        UmiProviderChatDestroy(plan);
        plan = NULL;
        strcpy(reply->text, "caller changed");
        CHECK(strstr(entry->request, "Original context") != NULL &&
              strstr(entry->reply, "Ignore this remainder") != NULL);
        OK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, sizeof(output)));
        CHECK(strstr(output, "caf\xc3\xa9") != NULL && strstr(output, "Ignore") == NULL);
    }
    else if (strcmp(name, "large") == 0)
    {
        char prompt[UMI_PROVIDER_CHAT_PROMPT_CAPACITY], context[UMI_PROVIDER_CHAT_CONTEXT_CAPACITY];
        memset(prompt, 'p', sizeof(prompt) - 1U);
        prompt[sizeof(prompt) - 1U] = '\0';
        memset(context, 'c', sizeof(context) - 1U);
        context[sizeof(context) - 1U] = '\0';
        UmiProviderChatDestroy(plan);
        plan = NULL;
        OK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, prompt, context, 128U,
                                  &plan));
        memset(reply->text, 'r', sizeof(reply->text) - 1U);
        reply->text[sizeof(reply->text) - 1U] = '\0';
        OK(UmiProviderChatHistoryAppend(history, plan, reply, &id));
        entry = UmiProviderChatHistoryAt(history, 1U);
        CHECK(strcmp(entry->request, UmiProviderChatInput(plan)) == 0 && strlen(entry->request) > 12000U);
        CHECK(strlen(entry->reply) == UMI_PROVIDER_CHAT_TEXT_CAPACITY - 1U &&
              strcmp(entry->reply, reply->text) == 0);
    }
    else if (strcmp(name, "full") == 0)
    {
        for (size_t i = 1U; i < UMI_PROVIDER_CHAT_HISTORY_CAPACITY; ++i)
            OK(UmiProviderChatHistoryAppend(history, plan, reply, &id));
        current = UmiProviderChatHistoryRevision(history);
        id = 999U;
        CHECK(UmiProviderChatHistoryAppend(history, plan, reply, &id) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(id == 999U && UmiProviderChatHistoryRevision(history) == current);
        CHECK(UmiProviderChatHistoryCount(history) == UMI_PROVIDER_CHAT_HISTORY_CAPACITY &&
              UmiProviderChatHistoryAt(history, 0U)->id == 1U);
    }
    else if (strcmp(name, "invalid") == 0)
    {
        /* Invalid responses never publish half an exchange or consume an ID. */
        const char *bad[] = {"", "bad\x1b", "\xc0\xaf", "\xed\xa0\x80", "\xf4\x90\x80\x80"};
        for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i)
        {
            strcpy(reply->text, bad[i]);
            id = 99U;
            CHECK(UmiProviderChatHistoryAppend(history, plan, reply, &id) != UMI_STATUS_OK);
            CHECK(id == 99U && UmiProviderChatHistoryCount(history) == 1U &&
                  UmiProviderChatHistoryRevision(history) == current);
        }
        memset(reply->text, 'x', sizeof(reply->text));
        CHECK(UmiProviderChatHistoryAppend(history, plan, reply, &id) == UMI_STATUS_CAPACITY_EXCEEDED);
        strcpy(reply->text, "valid");
        strcpy(reply->model, "\xff");
        CHECK(UmiProviderChatHistoryAppend(history, plan, reply, &id) != UMI_STATUS_OK);
        strcpy(reply->model, "reported-model");
        reply->http_status = 500U;
        CHECK(UmiProviderChatHistoryAppend(history, plan, reply, &id) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(id == 99U && history->next_id == 2U && history->count == 1U && history->revision == current);
    }
    else if (strcmp(name, "stale") == 0)
    {
        CHECK(UmiProviderChatHistoryClear(history, current - 1U) == UMI_STATUS_BUSY);
        CHECK(UmiProviderChatHistoryRemove(history, current - 1U, id) == UMI_STATUS_BUSY);
        CHECK(UmiProviderChatHistoryCompose(history, current - 1U, &excerpt, 1U, output, sizeof(output)) ==
              UMI_STATUS_BUSY);
        CHECK(strcmp(output, "unchanged") == 0 && history->count == 1U && history->revision == current);
    }
    else if (strcmp(name, "remove") == 0)
    {
        strcpy(reply->text, "second exchange");
        OK(UmiProviderChatHistoryAppend(history, plan, reply, &id));
        current = history->revision;
        CHECK(UmiProviderChatHistoryRemove(history, current, 99U) == UMI_STATUS_NOT_FOUND &&
              history->revision == current);
        OK(UmiProviderChatHistoryRemove(history, current, 1U));
        CHECK(history->count == 1U && history->entries[0].id == 2U &&
              strcmp(history->entries[0].reply, "second exchange") == 0);
        CHECK(Zero(&history->entries[1], sizeof(history->entries[1])));
        CHECK(UmiProviderChatHistoryCompose(history, history->revision, &excerpt, 1U, output,
                                            sizeof(output)) == UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(output, "unchanged") == 0);
    }
    else if (strcmp(name, "clear") == 0)
    {
        OK(UmiProviderChatHistoryClear(history, current));
        CHECK(history->count == 0U && Zero(history->entries, sizeof(history->entries)));
        OK(UmiProviderChatHistoryAppend(history, plan, reply, &id));
        CHECK(id == 2U);
        CHECK(UmiProviderChatHistoryCompose(history, history->revision, &excerpt, 1U, output,
                                            sizeof(output)) == UMI_STATUS_NOT_FOUND);
    }
    else if (strcmp(name, "compose") == 0)
    {
        OK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, sizeof(output)));
        CHECK(strcmp(output, "Earlier assistant text (quoted context):\ncaf\xc3\xa9") == 0);
        CHECK(history->revision == current && history->count == 1U);
    }
    else if (strcmp(name, "order") == 0)
    {
        UmiProviderChatHistoryExcerpt parts[] = {excerpt, {id, UMI_AI_ROLE_USER, 0U, 7U}};
        OK(UmiProviderChatHistoryCompose(history, current, parts, 2U, output, sizeof(output)));
        CHECK(strcmp(output, "Earlier assistant text (quoted context):\ncaf\xc3\xa9\n\nEarlier user text "
                             "(quoted context):\nExplain") == 0);
        CHECK(strstr(output, "Original context") == NULL && strstr(output, "reported-model") == NULL);
    }
    else if (strcmp(name, "utf8") == 0)
    {
        excerpt.byte_offset = 8U;
        excerpt.byte_count = 1U;
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, sizeof(output)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        excerpt.byte_offset = 9U;
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, sizeof(output)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(strcmp(output, "unchanged") == 0);
        excerpt.byte_offset = 8U;
        excerpt.byte_count = 2U;
        OK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, sizeof(output)));
        CHECK(strstr(output, "\xc3\xa9") != NULL);
    }
    else if (strcmp(name, "ranges") == 0)
    {
        size_t invalid[][2] = {{SIZE_MAX, 1U}, {1U, SIZE_MAX}, {strlen(entry->reply), 1U}, {0U, 0U}};
        for (size_t i = 0U; i < sizeof(invalid) / sizeof(invalid[0]); ++i)
        {
            excerpt.byte_offset = invalid[i][0];
            excerpt.byte_count = invalid[i][1];
            CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, sizeof(output)) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(strcmp(output, "unchanged") == 0);
        }
    }
    else if (strcmp(name, "roles") == 0)
    {
        const UmiAiRole roles[] = {UMI_AI_ROLE_SYSTEM, UMI_AI_ROLE_TOOL, (UmiAiRole)99};
        for (size_t i = 0U; i < sizeof(roles) / sizeof(roles[0]); ++i)
        {
            excerpt.role = roles[i];
            CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, sizeof(output)) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(strcmp(output, "unchanged") == 0);
        }
    }
    else if (strcmp(name, "atomic") == 0)
    {
        UmiProviderChatHistoryExcerpt parts[] = {excerpt, {99U, UMI_AI_ROLE_USER, 0U, 1U}};
        CHECK(UmiProviderChatHistoryCompose(history, current, parts, 2U, output, sizeof(output)) ==
              UMI_STATUS_NOT_FOUND);
        CHECK(strcmp(output, "unchanged") == 0);
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, 1U) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0);
        const char *wanted = "Earlier assistant text (quoted context):\ncaf\xc3\xa9";
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, strlen(wanted)) ==
              UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(output, "unchanged") == 0);
        OK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, strlen(wanted) + 1U));
        CHECK(strcmp(output, wanted) == 0);
    }
    else if (strcmp(name, "context-limit") == 0)
    {
        memset(reply->text, 'x', sizeof(reply->text) - 1U);
        reply->text[sizeof(reply->text) - 1U] = '\0';
        OK(UmiProviderChatHistoryAppend(history, plan, reply, &id));
        char *large = malloc(UMI_PROVIDER_CHAT_TEXT_CAPACITY);
        CHECK(large != NULL);
        strcpy(large, "unchanged");
        excerpt.entry_id = id;
        excerpt.byte_offset = 0U;
        excerpt.byte_count = strlen(reply->text);
        CHECK(UmiProviderChatHistoryCompose(history, history->revision, &excerpt, 1U, large,
                                            UMI_PROVIDER_CHAT_TEXT_CAPACITY) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(strcmp(large, "unchanged") == 0);
        free(large);
    }
    else if (strcmp(name, "flags") == 0)
    {
        for (unsigned flags = 1U; flags < 4U; ++flags)
        {
            reply->refused = (flags & 1U) != 0U;
            reply->truncated = (flags & 2U) != 0U;
            OK(UmiProviderChatHistoryAppend(history, plan, reply, &id));
            excerpt.entry_id = id;
            OK(UmiProviderChatHistoryCompose(history, history->revision, &excerpt, 1U, output,
                                             sizeof(output)));
            CHECK((strstr(output, "refusal") != NULL) == reply->refused);
            CHECK((strstr(output, "incomplete") != NULL) == reply->truncated);
        }
    }
    else if (strcmp(name, "exhausted") == 0)
    {
        /* Private counters let the regression reach overflow boundaries
         * without manufacturing billions of successful user operations. */
        history->next_id = UINT64_MAX;
        id = 99U;
        CHECK(UmiProviderChatHistoryAppend(history, plan, reply, &id) == UMI_STATUS_CAPACITY_EXCEEDED &&
              id == 99U);
        history->next_id = 2U;
        history->revision = UINT64_MAX;
        CHECK(UmiProviderChatHistoryAppend(history, plan, reply, &id) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiProviderChatHistoryRemove(history, UINT64_MAX, 1U) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(UmiProviderChatHistoryClear(history, UINT64_MAX) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(history->count == 1U && history->next_id == 2U && history->revision == UINT64_MAX);
    }
    else if (strcmp(name, "arguments") == 0)
    {
        CHECK(UmiProviderChatHistoryCreate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryCount(NULL) == 0U && UmiProviderChatHistoryRevision(NULL) == 0U &&
              UmiProviderChatHistoryAt(NULL, 0U) == NULL);
        CHECK(UmiProviderChatHistoryAppend(NULL, plan, reply, &id) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryAppend(history, NULL, reply, &id) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryAppend(history, plan, NULL, &id) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryAppend(history, plan, reply, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryRemove(history, current, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryClear(NULL, current) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryCompose(history, current, NULL, 1U, output, sizeof(output)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 0U, output, sizeof(output)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt,
                                            UMI_PROVIDER_CHAT_HISTORY_EXCERPTS + 1U, output,
                                            sizeof(output)) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, NULL, sizeof(output)) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiProviderChatHistoryCompose(history, current, &excerpt, 1U, output, 0U) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(history->count == 1U && history->revision == current && strcmp(output, "unchanged") == 0);
        UmiProviderChatHistoryDestroy(NULL);
    }
    UmiProviderChatHistoryDestroy(history);
    UmiProviderChatDestroy(plan);
    free(reply);
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    return 0;
}

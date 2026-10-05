/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_chat/test_plan.c
 * PURPOSE: Cover immutable review, scoped credentials, stale settings and single-attempt dispatch without a real service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../profile_secrets/fixture.h"
#include "../../src/provider_chat/internal.h"
static KeyFixture keys;
static UmiDataServer *server;
static UmiProviderConnections *store;
static UmiProviderConnection connection;
static uint64_t revision;
static UmiCancellationToken *token;
static unsigned factories, transfers;
static const char *scenario;
static UmiStatus Factory(const char *application, const char *profile, UmiProfileSecrets **out)
{
    ++factories;
    CHECK(strcmp(application, "studio") == 0 && strcmp(profile, "desktop") == 0);
    *out = KeyService(&keys, profile);
    if (strcmp(scenario, "changed-after-auth") == 0)
    {
        strcpy(connection.label, "Another writer");
        CHECK(UmiProviderConnectionsPut(store, &connection, true, revision, &revision) == UMI_STATUS_OK);
    }
    if (strcmp(scenario, "cancel-after-auth") == 0)
        umi_cancellation_token_request(token);
    return UMI_STATUS_OK;
}
static UmiStatus Transfer(const UmiProviderConnectionCheckPlan *plan, const char *body, const char *key,
                          const UmiCancellationToken *cancel, char *out, size_t capacity, unsigned *http)
{
    ++transfers;
    CHECK(!umi_cancellation_token_is_requested(cancel));
    CHECK(strcmp(key, plan->requires_credential ? KEY_VALUE : "") == 0);
    CHECK(strstr(body, "\"store\":false") != NULL && strstr(body, "\"stream\":false") != NULL);
    CHECK(strstr(body, "Explain this") != NULL && strstr(body, "Context supplied by the user") != NULL);
    CHECK(strstr(body, "project-only context") != NULL);
    CHECK(strstr(body, plan->requires_credential ? "max_output_tokens" : "max_tokens") != NULL);
    if (strcmp(scenario, "timeout") == 0)
        return UMI_STATUS_TIMEOUT;
    const char *reply =
        plan->requires_credential
            ? "{\"object\":\"response\",\"model\":\"reported-model\",\"status\":\"completed\",\"output\":[{"
              "\"type\":\"message\",\"role\":\"assistant\",\"status\":\"completed\",\"content\":[{\"type\":"
              "\"output_text\",\"text\":\"Review this reply.\"}]}]}"
            : "{\"object\":\"chat.completion\",\"model\":\"reported-model\",\"choices\":[{\"finish_reason\":"
              "\"stop\",\"message\":{\"role\":\"assistant\",\"content\":\"Review this reply.\"}}]}";
    CHECK(strlen(reply) < capacity);
    strcpy(out, reply);
    *http = 200U;
    return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    scenario = argv[1];
    bool local = strcmp(scenario, "local") == 0;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiProviderConnectionsOpen(server, "studio", "desktop", &store) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    strcpy(connection.id, "test");
    strcpy(connection.label, "Test");
    strcpy(connection.model, "test-model");
    strcpy(connection.provider_id, local ? "local-chat" : "openai");
    strcpy(connection.endpoint,
           local ? "http://127.0.0.1:8080/v1/chat/completions" : "https://api.openai.com/v1/responses");
    if (!local)
        strcpy(connection.secret_reference, "vault:test");
    connection.route = local ? UMI_PROVIDER_CONNECTION_LOOPBACK : UMI_PROVIDER_CONNECTION_HTTPS;
    connection.enabled = true;
    connection.timeout_ms = 30000U;
    CHECK(UmiProviderConnectionsPut(store, &connection, false, 0U, &revision) == UMI_STATUS_OK);
    UmiProfileSecrets *initial = KeyService(&keys, "desktop");
    CHECK(UmiProfileSecretsRegister(initial, KEY_PASSWORD) == UMI_STATUS_OK);
    CHECK(UmiProfileSecretsSet(initial, KEY_PASSWORD, "test", KEY_VALUE) == UMI_STATUS_OK);
    UmiProfileSecretsDestroy(initial);
    keys.reads = 0U;
    UmiProviderChatPlan *plan = NULL;
    char prompt[128] = "Explain this \"line\".\nSecond line.";
    CHECK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, prompt, "project-only context",
                                 128U, &plan) == UMI_STATUS_OK);
    strcpy(prompt, "Changed after review");
    CHECK(strstr(UmiProviderChatInput(plan), "Explain this") != NULL);
    CHECK(UmiProviderChatOutputLimit(plan) == 128U);
    bool approved = true;
    const char *password = KEY_PASSWORD;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(scenario, "remote") == 0 || local || strcmp(scenario, "immutable") == 0)
    {
    }
    else if (strcmp(scenario, "denied") == 0)
    {
        approved = false;
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(scenario, "wrong-password") == 0)
    {
        password = "wrong-test-password";
        expected = UMI_STATUS_PERMISSION_DENIED;
    }
    else if (strcmp(scenario, "stale") == 0)
    {
        CHECK(UmiProviderConnectionsPut(store, &connection, true, revision, &revision) == UMI_STATUS_OK);
        expected = UMI_STATUS_BUSY;
    }
    else if (strcmp(scenario, "changed-after-auth") == 0)
        expected = UMI_STATUS_BUSY;
    else if (strcmp(scenario, "cancel") == 0)
    {
        umi_cancellation_token_request(token);
        expected = UMI_STATUS_CANCELLED;
    }
    else if (strcmp(scenario, "cancel-after-auth") == 0)
        expected = UMI_STATUS_CANCELLED;
    else if (strcmp(scenario, "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    else if (strcmp(scenario, "invalid-input") == 0)
    {
        UmiProviderChatPlan *bad = NULL;
        CHECK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, " ", "", 128U, &bad) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, "ok", "", 15U, &bad) ==
                  UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, "bad\x01", "", 128U,
                                     &bad) == UMI_STATUS_INVALID_ARGUMENT &&
              bad == NULL);
        CHECK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, "bad\xc0\xaf", "", 128U,
                                     &bad) == UMI_STATUS_PARSE_ERROR &&
              bad == NULL);
    }
    else if (strcmp(scenario, "scope") == 0)
    {
        UmiProviderChatPlan *bad = NULL;
        CHECK(UmiProviderChatPrepare(store, "studio", "other", "test", revision, "ok", "", 128U, &bad) ==
                  UMI_STATUS_PERMISSION_DENIED &&
              bad == NULL);
    }
    else if (strcmp(scenario, "chat-format") == 0)
    {
        UmiProviderChatPlan *chat = NULL;
        strcpy(connection.endpoint, "https://api.openai.com/v1/chat/completions");
        CHECK(UmiProviderConnectionsPut(store, &connection, true, revision, &revision) == UMI_STATUS_OK);
        CHECK(UmiProviderChatPrepare(store, "studio", "desktop", "test", revision, "Check", "", 128U,
                                     &chat) == UMI_STATUS_OK);
        CHECK(strstr(chat->body, "max_completion_tokens") != NULL &&
              strstr(chat->body, "\"messages\"") != NULL);
        UmiProviderChatDestroy(chat);
        expected = UMI_STATUS_BUSY;
    }
    else
        return 2;
    UmiProviderChatResult *result = malloc(sizeof(*result));
    CHECK(result != NULL);
    memset(result, 0x7f, sizeof(*result));
    CHECK(UmiProviderChatRunWith(plan, store, approved, password, token, result, Factory, Transfer) ==
          expected);
    if (expected == UMI_STATUS_OK)
        CHECK(transfers == 1U && result->http_status == 200U &&
              strcmp(result->text, "Review this reply.") == 0 && factories == (local ? 0U : 1U));
    else
        CHECK(result->text[0] == '\0');
    if (strcmp(scenario, "denied") == 0)
    {
        CHECK(factories == 0U && transfers == 0U && AllZero((const char *)result, sizeof(*result)));
        CHECK(UmiProviderChatRunWith(plan, store, true, password, token, result, Factory, Transfer) ==
              UMI_STATUS_OK);
    }
    unsigned before = transfers;
    CHECK(UmiProviderChatRunWith(plan, store, true, password, token, result, Factory, Transfer) ==
              UMI_STATUS_INVALID_STATE &&
          transfers == before);
    CHECK(AllZero((const char *)result, sizeof(*result)));
    if (strcmp(scenario, "wrong-password") == 0)
        CHECK(keys.reads == 0U);
    if (strcmp(scenario, "stale") == 0 || strcmp(scenario, "cancel") == 0)
        CHECK(factories == 0U);
    UmiProviderChatDestroy(plan);
    free(result);
    umi_cancellation_token_destroy(token);
    UmiProviderConnectionsDestroy(store);
    umi_data_server_destroy(server);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connection_checks/test_check.c
 * PURPOSE: Verify review, authentication and dispatch ordering with isolated adapters.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../profile_secrets/fixture.h"
#include "../../src/provider_connection_checks/internal.h"
static KeyFixture keys;
static UmiDataServer *server;
static UmiProviderConnections *store;
static UmiProviderConnection connection;
static UmiCancellationToken *token;
static uint64_t revision;
static unsigned factory_calls, transport_calls;
static bool change_during_auth, cancel_during_auth;
static UmiProviderConnection Make(bool remote)
{
    UmiProviderConnection c = {0};
    strcpy(c.id, "personal"); strcpy(c.label, "Model check"); strcpy(c.model, "test-model");
    strcpy(c.provider_id, remote ? "openai" : "local-chat");
    strcpy(c.endpoint, remote ? "https://api.openai.com/v1/responses" : "http://127.0.0.1:8080/v1/chat/completions");
    if (remote) strcpy(c.secret_reference, "vault:personal-key");
    c.route = remote ? UMI_PROVIDER_CONNECTION_HTTPS : UMI_PROVIDER_CONNECTION_LOOPBACK;
    c.timeout_ms = 30000U; c.enabled = true; return c;
}
static UmiStatus Factory(const char *app, const char *profile, UmiProfileSecrets **out)
{
    ++factory_calls; CHECK(strcmp(app, "studio") == 0 && strcmp(profile, "desktop") == 0);
    *out = KeyService(&keys, profile);
    if (change_during_auth) {
        strcpy(connection.model, "changed-model");
        CHECK(UmiProviderConnectionsPut(store, &connection, true, revision, &revision) == UMI_STATUS_OK);
    }
    if (cancel_during_auth) umi_cancellation_token_request(token);
    return UMI_STATUS_OK;
}
static UmiStatus Transport(const UmiProviderConnectionCheckPlan *plan, const char *key,
    const UmiCancellationToken *cancel, UmiProviderConnectionCheckResult *out)
{
    ++transport_calls; CHECK(!umi_cancellation_token_is_requested(cancel));
    CHECK(strcmp(plan->connection.model, "test-model") == 0);
    CHECK(strcmp(key, plan->requires_credential ? KEY_VALUE : "") == 0);
    out->http_status = 200U; out->listed_models = 1U; out->model_listed = true; return UMI_STATUS_OK;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiProviderConnectionsOpen(server, "studio", "desktop", &store) == UMI_STATUS_OK);
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    bool local = strcmp(argv[1], "local") == 0;
    connection = Make(!local);
    CHECK(UmiProviderConnectionsPut(store, &connection, false, 0U, &revision) == UMI_STATUS_OK);
    UmiProfileSecrets *initial = KeyService(&keys, "desktop");
    CHECK(UmiProfileSecretsRegister(initial, KEY_PASSWORD) == UMI_STATUS_OK);
    CHECK(UmiProfileSecretsSet(initial, KEY_PASSWORD, "personal-key", KEY_VALUE) == UMI_STATUS_OK);
    UmiProfileSecretsDestroy(initial); keys.reads = 0U;
    UmiProviderConnectionCheckPlan plan = {0}; UmiProviderConnectionCheckResult result = {0};
    CHECK(UmiProviderConnectionCheckPrepare(store, "studio", "desktop", "personal", revision, &plan) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK; bool approved = true; const char *password = KEY_PASSWORD;
    if (strcmp(argv[1], "describe") == 0) {
        char endpoint[513]; bool remote = false;
        CHECK(UmiProviderConnectionCheckDescribe(&connection, endpoint, sizeof(endpoint), &remote) == UMI_STATUS_OK);
        CHECK(remote && strcmp(endpoint, "https://api.openai.com/v1/models") == 0);
        const char *bad[] = {"https://api.openai.com.evil.invalid/v1/responses", "https://api.openai.com:443/v1/responses",
            "https://api.openai.com/v1/models", "https://api.example.invalid/v1/responses"};
        for (size_t i = 0U; i < sizeof(bad)/sizeof(bad[0]); ++i) {
            UmiProviderConnection c = connection; strcpy(c.endpoint, bad[i]); strcpy(endpoint, "untouched");
            CHECK(UmiProviderConnectionCheckDescribe(&c, endpoint, sizeof(endpoint), &remote) != UMI_STATUS_OK);
            CHECK(strcmp(endpoint, "untouched") == 0);
        }
        UmiProviderConnection c = connection; strcpy(c.secret_reference, "environment:api-key");
        CHECK(UmiProviderConnectionCheckDescribe(&c, endpoint, sizeof(endpoint), &remote) == UMI_STATUS_PERMISSION_DENIED);
        c = connection; c.enabled = false;
        CHECK(UmiProviderConnectionCheckDescribe(&c, endpoint, sizeof(endpoint), &remote) == UMI_STATUS_PERMISSION_DENIED);
        c = connection; c.model[0] = '\0';
        CHECK(UmiProviderConnectionCheckDescribe(&c, endpoint, sizeof(endpoint), &remote) == UMI_STATUS_INVALID_ARGUMENT);
        c = Make(false); strcpy(c.endpoint, "http://127.0.0.1:8080/other");
        CHECK(UmiProviderConnectionCheckDescribe(&c, endpoint, sizeof(endpoint), &remote) == UMI_STATUS_PERMISSION_DENIED);
    } else if (strcmp(argv[1], "local") == 0 || strcmp(argv[1], "remote") == 0) {
        /* Only these success paths should reach transport. */
    } else if (strcmp(argv[1], "denied") == 0) { approved = false; expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(argv[1], "wrong-password") == 0) { password = "wrong-test-password"; expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(argv[1], "stale") == 0) {
        CHECK(UmiProviderConnectionsPut(store, &connection, true, revision, &revision) == UMI_STATUS_OK); expected = UMI_STATUS_BUSY;
    } else if (strcmp(argv[1], "changed-after-auth") == 0) { change_during_auth = true; expected = UMI_STATUS_BUSY;
    } else if (strcmp(argv[1], "cancelled") == 0) { umi_cancellation_token_request(token); expected = UMI_STATUS_CANCELLED;
    } else if (strcmp(argv[1], "cancelled-after-auth") == 0) { cancel_during_auth = true; expected = UMI_STATUS_CANCELLED;
    } else if (strcmp(argv[1], "tampered-plan") == 0) { strcpy(plan.check_endpoint, "https://other.invalid/v1/models"); expected = UMI_STATUS_BUSY;
    } else if (strcmp(argv[1], "scope") == 0) { strcpy(plan.profile, "another"); expected = UMI_STATUS_PERMISSION_DENIED;
    } else if (strcmp(argv[1], "header-injection") == 0) { strcpy(keys.value, "key\r\nInjected: value"); expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(argv[1], "missing-key") == 0) { keys.present = false; expected = UMI_STATUS_NOT_FOUND;
    } else { return 2; }
    memset(&result, 0x7f, sizeof(result));
    CHECK(UmiConnectionCheckRunWith(&plan, store, approved, password, token, &result, Factory, Transport) == expected);
    if (expected == UMI_STATUS_OK) {
        CHECK(transport_calls == 1U && result.http_status == 200U && result.model_listed);
        CHECK(factory_calls == (local ? 0U : 1U) && keys.reads == (local ? 0U : 1U));
    } else {
        CHECK(transport_calls == 0U && AllZero((const char *)&result, sizeof(result)));
        if (strcmp(argv[1], "wrong-password") == 0) CHECK(keys.reads == 0U);
        if (strcmp(argv[1], "stale") == 0 || strcmp(argv[1], "denied") == 0 ||
            strcmp(argv[1], "tampered-plan") == 0 || strcmp(argv[1], "scope") == 0) CHECK(factory_calls == 0U);
    }
    umi_cancellation_token_destroy(token); UmiProviderConnectionsDestroy(store); umi_data_server_destroy(server);
    return 0;
}

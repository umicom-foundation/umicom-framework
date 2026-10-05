/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connection_checks/check.c
 * PURPOSE: Bind model checks to reviewed settings and freshly verified credentials.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "internal.h"
#include "../provider_connections/internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

UmiStatus UmiProviderConnectionCheckDescribe(const UmiProviderConnection *connection,
    char *out_endpoint, size_t capacity, bool *out_requires_credential)
{
    if (out_endpoint == NULL || out_requires_credential == NULL || capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiProviderConnectionValidate(connection);
    if (status != UMI_STATUS_OK) return status;
    if (!connection->enabled) return UMI_STATUS_PERMISSION_DENIED;
    if (connection->model[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    char endpoint[UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY]; bool remote = false;
    if (strcmp(connection->provider_id, "openai") == 0) {
        /* Exact comparison prevents a saved lookalike host, path, user-info or
         * port from turning a model check into credential forwarding. */
        if (connection->route != UMI_PROVIDER_CONNECTION_HTTPS ||
            (strcmp(connection->endpoint, "https://api.openai.com/v1/responses") != 0 &&
             strcmp(connection->endpoint, "https://api.openai.com/v1/chat/completions") != 0) ||
            strncmp(connection->secret_reference, "vault:", 6U) != 0 ||
            connection->secret_reference[6] == '\0') return UMI_STATUS_PERMISSION_DENIED;
        strcpy(endpoint, "https://api.openai.com/v1/models"); remote = true;
    } else if (strcmp(connection->provider_id, "local-chat") == 0) {
        if (connection->route != UMI_PROVIDER_CONNECTION_LOOPBACK) return UMI_STATUS_PERMISSION_DENIED;
        const char *path = strchr(connection->endpoint + strlen("http://127.0.0.1:"), '/');
        if (path == NULL || strcmp(path, "/v1/chat/completions") != 0) return UMI_STATUS_PERMISSION_DENIED;
        size_t origin = (size_t)(path - connection->endpoint);
        memcpy(endpoint, connection->endpoint, origin); strcpy(endpoint + origin, "/v1/models");
    } else return UMI_STATUS_NOT_IMPLEMENTED;
    if (strlen(endpoint) >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(out_endpoint, endpoint); *out_requires_credential = remote;
    return UMI_STATUS_OK;
}
UmiStatus UmiProviderConnectionCheckPrepare(UmiProviderConnections *store,
    const char *application_id, const char *profile_name, const char *connection_id,
    uint64_t expected_revision, UmiProviderConnectionCheckPlan *out_plan)
{
    if (store == NULL || out_plan == NULL ||
        !UmiProviderConnectionIdValid(application_id, UMI_PROVIDER_CONNECTION_ID_CAPACITY) ||
        !UmiProviderConnectionIdValid(profile_name, UMI_PROVIDER_CONNECTION_ID_CAPACITY) ||
        !UmiProviderConnectionIdValid(connection_id, UMI_PROVIDER_CONNECTION_ID_CAPACITY) ||
        UmiProfileSecretsScopeValidate(application_id, profile_name) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    char prefix[128];
    int used = snprintf(prefix, sizeof(prefix), "provider.connections/%s/%s/", application_id, profile_name);
    /* The immutable private prefix ties the metadata and native secret scopes
     * together; a caller cannot supply another profile solely for its key. */
    if (used < 0 || (size_t)used >= sizeof(prefix) || strcmp(prefix, store->prefix) != 0)
        return UMI_STATUS_PERMISSION_DENIED;
    UmiProviderConnectionSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiProviderConnectionsRead(store, snapshot);
    if (status == UMI_STATUS_OK && snapshot->revision != expected_revision) status = UMI_STATUS_BUSY;
    UmiProviderConnectionCheckPlan plan = {0};
    if (status == UMI_STATUS_OK) {
        status = UMI_STATUS_NOT_FOUND;
        for (size_t i = 0U; i < snapshot->count; ++i) if (strcmp(snapshot->items[i].id, connection_id) == 0) {
            plan.connection = snapshot->items[i];
            status = UmiProviderConnectionCheckDescribe(&plan.connection, plan.check_endpoint,
                sizeof(plan.check_endpoint), &plan.requires_credential); break;
        }
    }
    if (status == UMI_STATUS_OK) {
        strcpy(plan.application, application_id); strcpy(plan.profile, profile_name);
        plan.revision = expected_revision; *out_plan = plan;
    }
    free(snapshot); return status;
}
static UmiStatus Current(const UmiProviderConnectionCheckPlan *plan, UmiProviderConnections *store)
{
    UmiProviderConnectionCheckPlan current = {0};
    if (plan == NULL || UmiProviderConnectionValidate(&plan->connection) != UMI_STATUS_OK ||
        memchr(plan->check_endpoint, '\0', sizeof(plan->check_endpoint)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiProviderConnectionCheckPrepare(store, plan->application, plan->profile,
        plan->connection.id, plan->revision, &current);
    if (status != UMI_STATUS_OK) return status;
    /* Canonical encoding compares logical fields rather than struct padding.
     * A mutated review plan is refused even when its revision is unchanged. */
    char expected[UMI_PROVIDER_CONNECTION_WIRE_CAPACITY], actual[UMI_PROVIDER_CONNECTION_WIRE_CAPACITY];
    status = UmiProviderConnectionEncode(&plan->connection, expected, sizeof(expected));
    if (status == UMI_STATUS_OK) status = UmiProviderConnectionEncode(&current.connection, actual, sizeof(actual));
    if (status == UMI_STATUS_OK && (strcmp(expected, actual) != 0 ||
        strcmp(plan->check_endpoint, current.check_endpoint) != 0 || plan->requires_credential != current.requires_credential))
        status = UMI_STATUS_BUSY;
    return status;
}
bool UmiConnectionCheckKeyValid(const char *key)
{
    if (key == NULL || key[0] == '\0') return false;
    for (size_t i = 0U; i < UMI_PLATFORM_SECRET_VALUE_CAPACITY; ++i) {
        unsigned char c = (unsigned char)key[i];
        if (c == 0U) return true;
        /* A bearer token cannot inject another HTTP header or contain spaces.
         * Provider key prefixes are deliberately not hard-coded. */
        if (c < 0x21U || c > 0x7eU) return false;
    }
    return false;
}
/* The catalogue-only coordinator is superseded by UmiConnectionAuthorize,
 * shared by catalogue checks and reviewed chat so both enforce fresh credentials
 * and the same saved-revision boundary.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiConnectionCheckRunWith(const UmiProviderConnectionCheckPlan *plan,
    UmiProviderConnections *store, bool approved, const char *password,
    const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out,
    UmiConnectionCheckSecretsFactory factory, UmiConnectionCheckTransport transport)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (transport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    UmiStatus status = Current(plan, store);
    char key[UMI_PLATFORM_SECRET_VALUE_CAPACITY] = {0}; UmiProfileSecrets *secrets = NULL;
    if (status == UMI_STATUS_OK && plan->requires_credential) {
        status = factory == NULL ? UMI_STATUS_INVALID_ARGUMENT : factory(plan->application, plan->profile, &secrets);
        if (status == UMI_STATUS_OK && secrets == NULL) status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK) status = UmiProfileSecretsGet(secrets, password,
            plan->connection.secret_reference + 6U, key, sizeof(key));
        if (status == UMI_STATUS_OK && !UmiConnectionCheckKeyValid(key)) status = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiProfileSecretsDestroy(secrets);
    /* Verification can take time. Recheck after it, without holding a database
     * transaction across either the credential callback or the network call. */
    if (status == UMI_STATUS_OK) status = Current(plan, store);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) status = transport(plan, key, cancellation, out);
    umi_secret_clear(key, sizeof(key));
    return status;
}
#endif
typedef struct CheckDispatch {
    UmiConnectionCheckTransport transport;
    UmiProviderConnectionCheckResult *result;
} CheckDispatch;
static UmiStatus DispatchCheck(const UmiProviderConnectionCheckPlan *plan, const char *key,
    const UmiCancellationToken *cancellation, void *context)
{
    CheckDispatch *check = context;
    return check->transport(plan, key, cancellation, check->result);
}
/* Credential access and stale-review checks live in one Framework owner.
 * Adding a request type must not duplicate or weaken this ordering. */
UmiStatus UmiConnectionAuthorize(const UmiProviderConnectionCheckPlan *plan,
    UmiProviderConnections *store, bool approved, const char *password,
    const UmiCancellationToken *cancellation, UmiConnectionCheckSecretsFactory factory,
    UmiConnectionAuthorizedDispatch dispatch, void *context)
{
    if (dispatch == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (umi_cancellation_token_is_requested(cancellation)) return UMI_STATUS_CANCELLED;
    UmiStatus status = Current(plan, store);
    char key[UMI_PLATFORM_SECRET_VALUE_CAPACITY] = {0}; UmiProfileSecrets *secrets = NULL;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK && plan->requires_credential) {
        status = factory == NULL ? UMI_STATUS_INVALID_ARGUMENT : factory(plan->application, plan->profile, &secrets);
        if (status == UMI_STATUS_OK && secrets == NULL) status = UMI_STATUS_INVALID_STATE;
        if (status == UMI_STATUS_OK) status = UmiProfileSecretsGet(secrets, password,
            plan->connection.secret_reference + 6U, key, sizeof(key));
        if (status == UMI_STATUS_OK && !UmiConnectionCheckKeyValid(key)) status = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiProfileSecretsDestroy(secrets);
    if (status == UMI_STATUS_OK) status = Current(plan, store);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancellation)) status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK) status = dispatch(plan, key, cancellation, context);
    umi_secret_clear(key, sizeof(key)); return status;
}
UmiStatus UmiConnectionCheckRunWith(const UmiProviderConnectionCheckPlan *plan,
    UmiProviderConnections *store, bool approved, const char *password,
    const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out,
    UmiConnectionCheckSecretsFactory factory, UmiConnectionCheckTransport transport)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (transport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    CheckDispatch check = {transport, out};
    return UmiConnectionAuthorize(plan, store, approved, password, cancellation, factory, DispatchCheck, &check);
}
UmiStatus UmiProviderConnectionCheckRun(const UmiProviderConnectionCheckPlan *plan,
    UmiProviderConnections *store, bool approved, const char *password,
    const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out)
{
    if (out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof(*out));
    if (!approved) return UMI_STATUS_PERMISSION_DENIED;
    if (!UmiProviderConnectionCheckAvailable()) return UMI_STATUS_UNAVAILABLE;
    return UmiConnectionCheckRunWith(plan, store, approved, password, cancellation, out,
        UmiProfileSecretsPlatform, UmiConnectionCheckHttp);
}

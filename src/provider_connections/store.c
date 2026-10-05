/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connections/store.c
 * PURPOSE: Keep shared provider settings atomic and guard reviewed credential acquisition.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* One small index plus separate records fits the Data Server's bounded values.
 * Both are changed in the same transaction; no cache is published before commit.
 * The scope revision remains even when its last connection is removed. */
static UmiStatus Key(UmiProviderConnections *store, const char *id, char *out, size_t capacity)
{
    int length = id != NULL ? snprintf(out, capacity, "%sentry/%s", store->prefix, id)
        : snprintf(out, capacity, "%scatalogue", store->prefix);
    return length >= 0 && (size_t)length < capacity ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
}
static UmiStatus Load(UmiProviderConnections *store, UmiProviderConnectionSnapshot *snapshot)
{
    char key[UMI_PROVIDER_CONNECTION_KEY_CAPACITY];
    char wire[UMI_PROVIDER_CONNECTION_WIRE_CAPACITY];
    UmiStatus status = Key(store, NULL, key, sizeof(key));
    if (status == UMI_STATUS_OK) status = umi_data_server_get(store->server, key, wire, sizeof(wire));
    if (status == UMI_STATUS_NOT_FOUND) return UMI_STATUS_OK;
    if (status == UMI_STATUS_OK) status = UmiProviderConnectionIndexDecode(wire, snapshot);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < snapshot->count; ++i) {
        char id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
        memcpy(id, snapshot->items[i].id, sizeof(id));
        status = Key(store, id, key, sizeof(key));
        if (status == UMI_STATUS_OK) status = umi_data_server_get(store->server, key, wire, sizeof(wire));
        if (status == UMI_STATUS_NOT_FOUND) status = UMI_STATUS_PARSE_ERROR;
        if (status == UMI_STATUS_OK) status = UmiProviderConnectionDecode(wire, &snapshot->items[i]);
        if (status == UMI_STATUS_OK && strcmp(id, snapshot->items[i].id) != 0) status = UMI_STATUS_PARSE_ERROR;
    }
    return status;
}
static UmiStatus Finish(UmiProviderConnections *store, UmiStatus status)
{
    if (status == UMI_STATUS_OK) status = umi_data_server_commit(store->server);
    if (status != UMI_STATUS_OK) {
        UmiStatus rollback = umi_data_server_rollback(store->server);
        if (rollback != UMI_STATUS_OK) {
            store->poisoned = true;
            return UMI_STATUS_INVALID_STATE;
        }
    }
    return status;
}
static UmiStatus Begin(UmiProviderConnections *store)
{
    if (store->poisoned) return UMI_STATUS_INVALID_STATE;
    /* A failed begin may mean another caller owns the transaction. It must
     * never be followed by our rollback, which would discard that caller's work. */
    return umi_data_server_begin(store->server);
}
UmiStatus UmiProviderConnectionsOpen(UmiDataServer *server,
    const char *application_id, const char *profile_id, UmiProviderConnections **out_store)
{
    if (server == NULL || out_store == NULL ||
        !UmiProviderConnectionIdValid(application_id, UMI_PROVIDER_CONNECTION_ID_CAPACITY) ||
        !UmiProviderConnectionIdValid(profile_id, UMI_PROVIDER_CONNECTION_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderConnections *store = calloc(1U, sizeof(*store));
    if (store == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    int length = snprintf(store->prefix, sizeof(store->prefix), "provider.connections/%s/%s/", application_id, profile_id);
    UmiStatus status = length >= 0 && (size_t)length < sizeof(store->prefix)
        ? umi_mutex_create(&store->mutex) : UMI_STATUS_CAPACITY_EXCEEDED;
    if (status != UMI_STATUS_OK) { free(store); return status; }
    store->server = server;
    *out_store = store;
    return UMI_STATUS_OK;
}
void UmiProviderConnectionsDestroy(UmiProviderConnections *store)
{
    if (store == NULL) return;
    umi_mutex_destroy(store->mutex);
    free(store);
}
UmiStatus UmiProviderConnectionsRead(UmiProviderConnections *store, UmiProviderConnectionSnapshot *out_snapshot)
{
    if (store == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderConnectionSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_mutex_lock(store->mutex);
    if (status == UMI_STATUS_OK) {
        status = Begin(store);
        if (status == UMI_STATUS_OK) status = Finish(store, Load(store, snapshot));
        if (status == UMI_STATUS_OK) *out_snapshot = *snapshot;
        (void)umi_mutex_unlock(store->mutex);
    }
    free(snapshot);
    return status;
}
static size_t Find(const UmiProviderConnectionSnapshot *snapshot, const char *id)
{
    for (size_t i = 0U; i < snapshot->count; ++i)
        if (strcmp(snapshot->items[i].id, id) == 0) return i;
    return snapshot->count;
}
/* A shared edit path keeps deletion and replacement under the same concurrency
 * rules. Credential removal is intentionally separate: another connection may
 * still reference the same key, and two stores cannot promise one transaction. */
static UmiStatus Edit(UmiProviderConnections *store, const char *id,
    const UmiProviderConnection *connection, bool replace_existing,
    uint64_t expected_revision, uint64_t *out_revision)
{
    UmiProviderConnectionSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = umi_mutex_lock(store->mutex);
    if (status != UMI_STATUS_OK) { free(snapshot); return status; }
    status = Begin(store);
    if (status != UMI_STATUS_OK) {
        (void)umi_mutex_unlock(store->mutex);
        free(snapshot);
        return status;
    }
    status = Load(store, snapshot);
    if (status == UMI_STATUS_OK && snapshot->revision != expected_revision) status = UMI_STATUS_BUSY;
    if (status == UMI_STATUS_OK && snapshot->revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
    size_t index = Find(snapshot, id);
    bool exists = index < snapshot->count;
    if (status == UMI_STATUS_OK && !exists && (connection == NULL || replace_existing)) status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK && exists && connection != NULL && !replace_existing) status = UMI_STATUS_ALREADY_EXISTS;
    if (status == UMI_STATUS_OK && !exists && snapshot->count == UMI_PROVIDER_CONNECTION_LIMIT) status = UMI_STATUS_CAPACITY_EXCEEDED;
    char key[UMI_PROVIDER_CONNECTION_KEY_CAPACITY];
    char wire[UMI_PROVIDER_CONNECTION_WIRE_CAPACITY];
    if (status == UMI_STATUS_OK) status = Key(store, id, key, sizeof(key));
    if (status == UMI_STATUS_OK && connection != NULL && !exists) {
        /* Refuse to overwrite an orphan if an external tool damaged the index.
         * Keep the evidence in place for recovery instead of hiding corruption. */
        UmiStatus probe = umi_data_server_get(store->server, key, wire, sizeof(wire));
        status = probe == UMI_STATUS_NOT_FOUND ? UMI_STATUS_OK :
            probe == UMI_STATUS_OK ? UMI_STATUS_PARSE_ERROR : probe;
    }
    if (status == UMI_STATUS_OK && connection != NULL) {
        status = UmiProviderConnectionEncode(connection, wire, sizeof(wire));
        if (status == UMI_STATUS_OK) status = umi_data_server_set(store->server, key, wire);
        if (status == UMI_STATUS_OK) {
            snapshot->items[index] = *connection;
            if (!exists) ++snapshot->count;
        }
    } else if (status == UMI_STATUS_OK) {
        status = umi_data_server_delete(store->server, key);
        if (status == UMI_STATUS_OK) {
            for (size_t i = index + 1U; i < snapshot->count; ++i) snapshot->items[i - 1U] = snapshot->items[i];
            --snapshot->count;
        }
    }
    if (status == UMI_STATUS_OK) {
        ++snapshot->revision;
        status = UmiProviderConnectionIndexEncode(snapshot, wire, sizeof(wire));
        if (status == UMI_STATUS_OK) status = Key(store, NULL, key, sizeof(key));
        if (status == UMI_STATUS_OK) status = umi_data_server_set(store->server, key, wire);
    }
    status = Finish(store, status);
    if (status == UMI_STATUS_OK) *out_revision = snapshot->revision;
    (void)umi_mutex_unlock(store->mutex);
    free(snapshot);
    return status;
}
UmiStatus UmiProviderConnectionsPut(UmiProviderConnections *store,
    const UmiProviderConnection *connection, bool replace_existing,
    uint64_t expected_revision, uint64_t *out_revision)
{
    if (store == NULL || out_revision == NULL || UmiProviderConnectionValidate(connection) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    return Edit(store, connection->id, connection, replace_existing, expected_revision, out_revision);
}
UmiStatus UmiProviderConnectionsRemove(UmiProviderConnections *store,
    const char *connection_id, uint64_t expected_revision, uint64_t *out_revision)
{
    if (store == NULL || out_revision == NULL ||
        !UmiProviderConnectionIdValid(connection_id, UMI_PROVIDER_CONNECTION_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    return Edit(store, connection_id, NULL, true, expected_revision, out_revision);
}
UmiStatus UmiProviderConnectionsAcquire(UmiProviderConnections *store,
    const char *connection_id, uint64_t expected_revision,
    const UmiSecretProviderRegistry *registry, UmiProviderConnection *out_connection,
    char *out_secret, size_t secret_capacity)
{
    if (out_secret != NULL && secret_capacity != 0U) umi_secret_clear(out_secret, secret_capacity);
    if (store == NULL || out_connection == NULL || out_secret == NULL || secret_capacity == 0U ||
        !UmiProviderConnectionIdValid(connection_id, UMI_PROVIDER_CONNECTION_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    UmiProviderConnectionSnapshot *snapshot = calloc(1U, sizeof(*snapshot));
    if (snapshot == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiProviderConnectionsRead(store, snapshot);
    if (status == UMI_STATUS_OK && snapshot->revision != expected_revision) status = UMI_STATUS_BUSY;
    size_t index = Find(snapshot, connection_id);
    if (status == UMI_STATUS_OK && index == snapshot->count) status = UMI_STATUS_NOT_FOUND;
    if (status == UMI_STATUS_OK && !snapshot->items[index].enabled) status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && snapshot->items[index].secret_reference[0] != '\0') {
        status = registry != NULL ? umi_secret_provider_registry_resolve(registry,
            snapshot->items[index].secret_reference, out_secret, secret_capacity) : UMI_STATUS_UNAVAILABLE;
        /* An adapter must return a complete value. Never expose a partial key
         * left behind by an adapter that failed or omitted its terminator. */
        if (status == UMI_STATUS_OK && (out_secret[0] == '\0' || memchr(out_secret, '\0', secret_capacity) == NULL))
            status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK) *out_connection = snapshot->items[index];
    else umi_secret_clear(out_secret, secret_capacity);
    free(snapshot);
    return status;
}

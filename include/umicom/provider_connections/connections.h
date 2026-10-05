/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/provider_connections/connections.h
 * PURPOSE: Persist reviewed connection settings without storing credential values.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PROVIDER_CONNECTIONS_CONNECTIONS_H
#define UMICOM_PROVIDER_CONNECTIONS_CONNECTIONS_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "umicom/data/data_server.h"
#include "umicom/security/secret_provider.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PROVIDER_CONNECTION_ID_CAPACITY 49U
#define UMI_PROVIDER_CONNECTION_LABEL_CAPACITY 161U
#define UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY 513U
#define UMI_PROVIDER_CONNECTION_MODEL_CAPACITY 129U
#define UMI_PROVIDER_CONNECTION_REFERENCE_CAPACITY 193U
#define UMI_PROVIDER_CONNECTION_LIMIT 32U

/* These routes describe configuration, not a successful connection. Remote
 * adapters must still approve the provider's host, verify TLS, prevent redirects
 * from forwarding credentials, and implement that provider's authentication. */
typedef enum UmiProviderConnectionRoute {
    UMI_PROVIDER_CONNECTION_HTTPS = 1,
    UMI_PROVIDER_CONNECTION_LOOPBACK = 2
} UmiProviderConnectionRoute;

/* Only non-secret settings belong here. Never put a key, password or signed
 * download URL into a label, model identifier, endpoint or credential reference.
 * A reference such as vault:personal-key names a separately stored credential. */
typedef struct UmiProviderConnection {
    char id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char provider_id[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    char label[UMI_PROVIDER_CONNECTION_LABEL_CAPACITY];
    char endpoint[UMI_PROVIDER_CONNECTION_ENDPOINT_CAPACITY];
    char model[UMI_PROVIDER_CONNECTION_MODEL_CAPACITY];
    char secret_reference[UMI_PROVIDER_CONNECTION_REFERENCE_CAPACITY];
    UmiProviderConnectionRoute route;
    uint32_t timeout_ms;
    bool enabled;
} UmiProviderConnection;

/* A scope-wide revision is a review token. Any saved edit invalidates an older
 * token, including removing and recreating the same connection identifier.
 * Allocate large snapshots on the heap in applications with small stacks. */
typedef struct UmiProviderConnectionSnapshot {
    uint64_t revision;
    size_t count;
    UmiProviderConnection items[UMI_PROVIDER_CONNECTION_LIMIT];
} UmiProviderConnectionSnapshot;
typedef struct UmiProviderConnections UmiProviderConnections;

/* IDs start with a lower-case ASCII letter and use letters, digits, '.', '_'
 * or '-'. Labels accept single-line UTF-8. HTTPS endpoints have a DNS host and
 * optional port/path; query strings, fragments, user information, percent
 * escapes and dot path segments are rejected. Loopback endpoints use exactly
 * http://127.0.0.1:<port> with a port from 1024 to 65535, and cannot use secrets.
 * Syntax validation does not establish trust in a host or model. */
UmiStatus UmiProviderConnectionValidate(const UmiProviderConnection *connection);

/* The caller supplies an already opened Data Server. Use an absolute SQLite
 * path chosen by the host for durable settings; a memory server is temporary.
 * Application/profile IDs isolate metadata but do not enforce user access.
 * Keep the borrowed server alive until all stores and workers are destroyed.
 * Open does not read a vault, authenticate a user, or contact a provider. */
UmiStatus UmiProviderConnectionsOpen(UmiDataServer *server,
    const char *application_id, const char *profile_id,
    UmiProviderConnections **out_store);
/* Join users before destruction. Removing the store never removes credentials. */
void UmiProviderConnectionsDestroy(UmiProviderConnections *store);
/* Outputs other than secret buffers remain unchanged on failure. Reads and
 * writes use Data Server transactions and serialize calls on this store.
 * A rollback failure poisons the store: close it and recover/reopen its server
 * before creating another store. Never silently continue after that failure. */
UmiStatus UmiProviderConnectionsRead(UmiProviderConnections *store,
    UmiProviderConnectionSnapshot *out_snapshot);
/* Explicit create/replace mode prevents a typo from creating a second profile.
 * BUSY means the saved revision changed: reread and review before trying again.
 * Updates do not create, replace or delete the referenced credential. */
UmiStatus UmiProviderConnectionsPut(UmiProviderConnections *store,
    const UmiProviderConnection *connection, bool replace_existing,
    uint64_t expected_revision, uint64_t *out_revision);
UmiStatus UmiProviderConnectionsRemove(UmiProviderConnections *store,
    const char *connection_id, uint64_t expected_revision, uint64_t *out_revision);

/* Acquire the exact enabled settings the user reviewed and, only then, resolve
 * their reference. The host must first unlock its local profile and approve
 * the destination through a provider-specific adapter. This is not a login or
 * a network call. Registry callbacks run after the database transaction ends.
 * Use the returned settings with the returned secret; do not reread a different
 * endpoint. Later edits affect future acquisitions, not a request in progress.
 * Buffers must not overlap. The entire secret buffer is cleared on failure;
 * after success the caller must clear it with umi_secret_clear after use.
 * The registry is borrowed and must belong to this application/profile scope. */
UmiStatus UmiProviderConnectionsAcquire(UmiProviderConnections *store,
    const char *connection_id, uint64_t expected_revision,
    const UmiSecretProviderRegistry *registry, UmiProviderConnection *out_connection,
    char *out_secret, size_t secret_capacity);
#ifdef __cplusplus
}
#endif
#endif

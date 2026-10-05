/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/provider_connections/test_connections.c
 * PURPOSE: Exercise durable settings, reviewed acquisition and transactional recovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/provider_connections/connections.h"
#include "umicom/base/version.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Assertions stay enabled in every build. Fixtures never use real credentials,
 * start a local model, or contact a remote service. SQLite files are confined
 * to a separate CTest working directory for each case. */
#define CHECK(condition) do { if (!(condition)) { fprintf(stderr, "line %d: %s\n", __LINE__, #condition); return 1; } } while (0)
#define OK(expression) CHECK((expression) == UMI_STATUS_OK)
typedef struct Fixture {
    UmiDataServer *server;
    UmiProviderConnections *store, *other;
    UmiSecretProviderRegistry *registry;
    UmiProviderConnectionSnapshot snapshot, untouched;
    UmiProviderConnection connection, output;
    uint64_t revision;
    unsigned secret_calls;
    int secret_mode;
} Fixture;
static Fixture f;
static UmiProviderConnection Connection(void)
{
    UmiProviderConnection result = {0};
    strcpy(result.id, "personal");
    strcpy(result.provider_id, "example");
    strcpy(result.label, "Personal inference");
    strcpy(result.endpoint, "https://api.example.invalid/inference");
    strcpy(result.model, "example/model");
    strcpy(result.secret_reference, "vault:personal-key");
    result.route = UMI_PROVIDER_CONNECTION_HTTPS;
    result.timeout_ms = 30000U;
    result.enabled = true;
    return result;
}
static int Start(bool sqlite)
{
    UmiStatus status;
    if (sqlite) {
        (void)remove("connections.sqlite3");
        status = umi_data_server_create_sqlite("connections.sqlite3", &f.server);
        if (status == UMI_STATUS_UNAVAILABLE) return 77;
    } else status = umi_data_server_create_memory(&f.server);
    OK(status);
    OK(UmiProviderConnectionsOpen(f.server, "studio", "owner", &f.store));
    f.connection = Connection();
    return 0;
}
static UmiStatus GetSecret(void *context, const char *name, char *out, size_t capacity)
{
    Fixture *fixture = context;
    ++fixture->secret_calls;
    if (strcmp(name, "personal-key") != 0) return UMI_STATUS_NOT_FOUND;
    /* A callback may inspect metadata. Acquisition must have released the
     * database transaction and store mutex before reaching this provider. */
    UmiStatus status = UmiProviderConnectionsRead(fixture->store, &fixture->snapshot);
    if (status != UMI_STATUS_OK) return status;
    if (fixture->secret_mode != 0) {
        memset(out, 'x', capacity);
        return fixture->secret_mode == 1 ? UMI_STATUS_IO_ERROR : UMI_STATUS_OK;
    }
    if (capacity < sizeof("fixture-credential")) return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out, "fixture-credential", sizeof("fixture-credential"));
    return UMI_STATUS_OK;
}
static int Registry(void)
{
    UmiSecretProvider provider = {0};
    provider.structure_size = (uint32_t)sizeof(provider);
    provider.abi_version = UMICOM_FRAMEWORK_ABI_VERSION;
    provider.instance = &f;
    provider.get = GetSecret;
    OK(umi_secret_provider_registry_create(&f.registry));
    OK(umi_secret_provider_registry_add(f.registry, "vault", &provider));
    return 0;
}
static bool Cleared(const char *value, size_t capacity)
{
    for (size_t i = 0U; i < capacity; ++i) if (value[i] != '\0') return false;
    return true;
}
static int Validation(void)
{
    UmiProviderConnection value = Connection();
    OK(UmiProviderConnectionValidate(&value));
    const char *bad[] = {"http://api.example.invalid", "https://user:password@host.test/path",
        "https://host.test/path?api_key=value", "https://host.test/#fragment",
        "https://host.test/%2fprivate", "https://host.test/../private", "https://host.test/./private",
        "https://host.test:65536/path", "https://host.test:00080/path", "https://bad-.test/path",
        "https://host..test/path", "https://host.test/path\\other", "https://host.test//path"};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
        strcpy(value.endpoint, bad[i]);
        CHECK(UmiProviderConnectionValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
    }
    value = Connection();
    strcpy(value.label, "Caf\xc3\xa9 profile");
    OK(UmiProviderConnectionValidate(&value));
    strcpy(value.label, "\xed\xa0\x80");
    CHECK(UmiProviderConnectionValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
    value = Connection();
    memset(value.model, 'a', sizeof(value.model));
    CHECK(UmiProviderConnectionValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
    value = Connection();
    strcpy(value.secret_reference, "vault://personal-key");
    CHECK(UmiProviderConnectionValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
    value = Connection();
    value.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    strcpy(value.endpoint, "http://127.0.0.1:8080/v1/chat/completions");
    CHECK(UmiProviderConnectionValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
    value.secret_reference[0] = '\0';
    OK(UmiProviderConnectionValidate(&value));
    strcpy(value.endpoint, "http://localhost:8080/v1");
    CHECK(UmiProviderConnectionValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
static int Lifecycle(void)
{
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 0U && f.snapshot.revision == 0U);
    strcpy(f.connection.label, "Caf\xc3\xa9 profile");
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    CHECK(f.revision == 1U);
    CHECK(UmiProviderConnectionsPut(f.store, &f.connection, false, 1U, &f.revision) == UMI_STATUS_ALREADY_EXISTS);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 1U && strcmp(f.snapshot.items[0].label, f.connection.label) == 0);
    f.connection.enabled = false;
    OK(UmiProviderConnectionsPut(f.store, &f.connection, true, 1U, &f.revision));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.revision == 2U && !f.snapshot.items[0].enabled);
    OK(UmiProviderConnectionsRemove(f.store, "personal", 2U, &f.revision));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 0U && f.snapshot.revision == 3U);
    CHECK(UmiProviderConnectionsRemove(f.store, "personal", 3U, &f.revision) == UMI_STATUS_NOT_FOUND);
    CHECK(f.revision == 3U);
    return 0;
}
static int Stale(void)
{
    OK(UmiProviderConnectionsOpen(f.server, "studio", "owner", &f.other));
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    uint64_t output = 98U;
    CHECK(UmiProviderConnectionsPut(f.other, &f.connection, true, 0U, &output) == UMI_STATUS_BUSY);
    CHECK(output == 98U);
    OK(UmiProviderConnectionsRemove(f.store, "personal", 1U, &f.revision));
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 2U, &f.revision));
    CHECK(UmiProviderConnectionsRemove(f.other, "personal", 1U, &output) == UMI_STATUS_BUSY);
    OK(UmiProviderConnectionsRead(f.other, &f.snapshot));
    CHECK(f.snapshot.count == 1U && f.snapshot.revision == 3U);
    return 0;
}
static int Scope(void)
{
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    const char *applications[] = {"media", "studio"};
    const char *profiles[] = {"owner", "guest"};
    for (size_t i = 0U; i < 2U; ++i) {
        OK(UmiProviderConnectionsOpen(f.server, applications[i], profiles[i], &f.other));
        OK(UmiProviderConnectionsRead(f.other, &f.snapshot));
        CHECK(f.snapshot.count == 0U && f.snapshot.revision == 0U);
        OK(UmiProviderConnectionsPut(f.other, &f.connection, false, 0U, &f.revision));
        UmiProviderConnectionsDestroy(f.other); f.other = NULL;
    }
    CHECK(UmiProviderConnectionsOpen(f.server, "../studio", "owner", &f.other) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(f.other == NULL);
    return 0;
}
static int Limit(void)
{
    for (size_t i = 0U; i < UMI_PROVIDER_CONNECTION_LIMIT; ++i) {
        (void)snprintf(f.connection.id, sizeof(f.connection.id), "profile-%zu", i);
        OK(UmiProviderConnectionsPut(f.store, &f.connection, false, f.revision, &f.revision));
    }
    strcpy(f.connection.id, "overflow");
    CHECK(UmiProviderConnectionsPut(f.store, &f.connection, false, f.revision, &f.revision) == UMI_STATUS_CAPACITY_EXCEEDED);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == UMI_PROVIDER_CONNECTION_LIMIT && f.revision == UMI_PROVIDER_CONNECTION_LIMIT);
    return 0;
}
static int Acquire(void)
{
    CHECK(Registry() == 0);
    char secret[64];
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    CHECK(f.secret_calls == 0U);
    OK(UmiProviderConnectionsAcquire(f.store, "personal", 1U, f.registry, &f.output, secret, sizeof(secret)));
    CHECK(strcmp(secret, "fixture-credential") == 0 && f.secret_calls == 1U);
    CHECK(strcmp(f.output.endpoint, f.connection.endpoint) == 0);
    umi_secret_clear(secret, sizeof(secret));
    f.connection.enabled = false;
    OK(UmiProviderConnectionsPut(f.store, &f.connection, true, 1U, &f.revision));
    memset(secret, 'z', sizeof(secret));
    CHECK(UmiProviderConnectionsAcquire(f.store, "personal", 1U, f.registry, &f.output, secret, sizeof(secret)) == UMI_STATUS_BUSY);
    CHECK(Cleared(secret, sizeof(secret)) && f.secret_calls == 1U);
    CHECK(UmiProviderConnectionsAcquire(f.store, "personal", 2U, f.registry, &f.output, secret, sizeof(secret)) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(f.secret_calls == 1U);
    f.connection.route = UMI_PROVIDER_CONNECTION_LOOPBACK;
    strcpy(f.connection.endpoint, "http://127.0.0.1:8080/v1");
    f.connection.secret_reference[0] = '\0'; f.connection.enabled = true;
    OK(UmiProviderConnectionsPut(f.store, &f.connection, true, 2U, &f.revision));
    OK(UmiProviderConnectionsAcquire(f.store, "personal", 3U, NULL, &f.output, secret, sizeof(secret)));
    CHECK(Cleared(secret, sizeof(secret)) && f.secret_calls == 1U);
    return 0;
}
static int FailedSecret(void)
{
    CHECK(Registry() == 0);
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    char secret[64];
    f.output = f.connection;
    strcpy(f.output.label, "Unchanged");
    for (int mode = 1; mode <= 2; ++mode) {
        f.secret_mode = mode;
        memset(secret, 'z', sizeof(secret));
        CHECK(UmiProviderConnectionsAcquire(f.store, "personal", 1U, f.registry, &f.output, secret, sizeof(secret)) != UMI_STATUS_OK);
        CHECK(Cleared(secret, sizeof(secret)) && strcmp(f.output.label, "Unchanged") == 0);
    }
    f.secret_mode = 0;
    CHECK(UmiProviderConnectionsAcquire(f.store, "personal", 1U, f.registry, &f.output, secret, 2U) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(Cleared(secret, 2U));
    CHECK(UmiProviderConnectionsAcquire(f.store, "personal", 1U, NULL, &f.output, secret, sizeof(secret)) == UMI_STATUS_UNAVAILABLE);
    CHECK(Cleared(secret, sizeof(secret)));
    return 0;
}
static int Transaction(void)
{
    OK(umi_data_server_begin(f.server));
    OK(umi_data_server_set(f.server, "caller-owned", "keep"));
    CHECK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision) == UMI_STATUS_BUSY);
    CHECK(umi_data_server_in_transaction(f.server));
    OK(umi_data_server_commit(f.server));
    char value[32];
    OK(umi_data_server_get(f.server, "caller-owned", value, sizeof(value)));
    CHECK(strcmp(value, "keep") == 0);
    return 0;
}
static int Corrupt(void)
{
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot)); memcpy(&f.untouched, &f.snapshot, sizeof(f.snapshot));
    const char *bad[] = {"catalogue\n1\n2\npersonal\npersonal\n", "catalogue\n0\n0\n",
        "catalogue\n1\n33\n", "catalogue\n18446744073709551616\n0\n",
        "catalogue\n01\n0\n", "catalogue\n1\n1\nmissing\n", "catalogue\n1\n0\nextra"};
    for (size_t i = 0U; i < sizeof(bad) / sizeof(bad[0]); ++i) {
        OK(umi_data_server_set(f.server, "provider.connections/studio/owner/catalogue", bad[i]));
        CHECK(UmiProviderConnectionsRead(f.store, &f.snapshot) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&f.snapshot, &f.untouched, sizeof(f.snapshot)) == 0);
    }
    OK(umi_data_server_set(f.server, "provider.connections/studio/owner/catalogue", "catalogue\n1\n1\npersonal\n"));
    const char *records[] = {"connection\n1\n30000\n1\n00\n", "connection\n1\n30000\n1\n7", "connection\n9\n"};
    for (size_t i = 0U; i < sizeof(records) / sizeof(records[0]); ++i) {
        OK(umi_data_server_set(f.server, "provider.connections/studio/owner/entry/personal", records[i]));
        CHECK(UmiProviderConnectionsRead(f.store, &f.snapshot) == UMI_STATUS_PARSE_ERROR);
        CHECK(memcmp(&f.snapshot, &f.untouched, sizeof(f.snapshot)) == 0);
    }
    return 0;
}
static int Reopen(void)
{
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    UmiProviderConnectionsDestroy(f.store); f.store = NULL;
    umi_data_server_destroy(f.server); f.server = NULL;
    OK(umi_data_server_create_sqlite("connections.sqlite3", &f.server));
    OK(UmiProviderConnectionsOpen(f.server, "studio", "owner", &f.store));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 1U && f.snapshot.revision == 1U);
    CHECK(strcmp(f.snapshot.items[0].secret_reference, "vault:personal-key") == 0);
    return 0;
}
/* Abort the index write after the entry was written or removed. A successful
 * rollback must restore both records and leave the revision output untouched. */
static int FailedSave(bool deleting)
{
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    OK(umi_data_server_execute(f.server,
        "CREATE TRIGGER reject_catalogue BEFORE INSERT ON umicom_kv "
        "WHEN NEW.key='provider.connections/studio/owner/catalogue' "
        "BEGIN SELECT RAISE(ABORT,'fixture write rejected'); END;"));
    strcpy(f.connection.label, "Must not persist");
    UmiStatus status = deleting ? UmiProviderConnectionsRemove(f.store, "personal", 1U, &f.revision)
        : UmiProviderConnectionsPut(f.store, &f.connection, true, 1U, &f.revision);
    CHECK(status != UMI_STATUS_OK && f.revision == 1U);
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 1U && f.snapshot.revision == 1U);
    CHECK(strcmp(f.snapshot.items[0].label, "Personal inference") == 0);
    OK(umi_data_server_execute(f.server, "DROP TRIGGER reject_catalogue;"));
    OK(UmiProviderConnectionsRemove(f.store, "personal", 1U, &f.revision));
    CHECK(f.revision == 2U);
    return 0;
}

static int Boundaries(void)
{
    UmiProviderConnection value = Connection();
    memset(value.id, 'a', sizeof(value.id) - 1U); value.id[sizeof(value.id) - 1U] = '\0';
    memset(value.provider_id, 'p', sizeof(value.provider_id) - 1U); value.provider_id[sizeof(value.provider_id) - 1U] = '\0';
    memset(value.label, 'L', sizeof(value.label) - 1U); value.label[sizeof(value.label) - 1U] = '\0';
    memset(value.model, 'm', sizeof(value.model) - 1U); value.model[sizeof(value.model) - 1U] = '\0';
    memset(value.secret_reference, 'p', 95U); value.secret_reference[95] = ':';
    memset(value.secret_reference + 96, 'k', 96U); value.secret_reference[192] = '\0';
    strcpy(value.endpoint, "https://example.invalid/");
    size_t prefix = strlen(value.endpoint);
    memset(value.endpoint + prefix, 'a', sizeof(value.endpoint) - prefix - 1U);
    value.endpoint[sizeof(value.endpoint) - 1U] = '\0';
    char scope[UMI_PROVIDER_CONNECTION_ID_CAPACITY];
    memset(scope, 's', sizeof(scope) - 1U); scope[sizeof(scope) - 1U] = '\0';
    OK(UmiProviderConnectionsOpen(f.server, scope, scope, &f.other));
    OK(UmiProviderConnectionValidate(&value));
    OK(UmiProviderConnectionsPut(f.other, &value, false, 0U, &f.revision));
    OK(UmiProviderConnectionsRead(f.other, &f.snapshot));
    CHECK(strcmp(f.snapshot.items[0].endpoint, value.endpoint) == 0);
    CHECK(strcmp(f.snapshot.items[0].secret_reference, value.secret_reference) == 0);
    CHECK(strcmp(f.snapshot.items[0].label, value.label) == 0);
    value.id[sizeof(value.id) - 1U] = 'a';
    CHECK(UmiProviderConnectionValidate(&value) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
static int Orphan(void)
{
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    OK(umi_data_server_delete(f.server, "provider.connections/studio/owner/catalogue"));
    CHECK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision) == UMI_STATUS_PARSE_ERROR);
    CHECK(f.revision == 1U);
    char wire[4096];
    OK(umi_data_server_get(f.server, "provider.connections/studio/owner/entry/personal", wire, sizeof(wire)));
    CHECK(strncmp(wire, "connection\n", 11U) == 0);
    OK(umi_data_server_set(f.server, "provider.connections/studio/owner/catalogue", "catalogue\n18446744073709551615\n1\npersonal\n"));
    CHECK(UmiProviderConnectionsRemove(f.store, "personal", UINT64_MAX, &f.revision) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(f.revision == 1U);
    return 0;
}
/* SQLite can end a transaction itself after RAISE(ROLLBACK). Data Server
 * reports that loss, and the settings store must refuse further work until
 * the owner deliberately replaces it. No unconfirmed edit may be published. */
static int LostTransaction(void)
{
    OK(UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision));
    OK(umi_data_server_execute(f.server,
        "CREATE TRIGGER lose_catalogue BEFORE INSERT ON umicom_kv "
        "WHEN NEW.key='provider.connections/studio/owner/catalogue' "
        "BEGIN SELECT RAISE(ROLLBACK,'fixture transaction lost'); END;"));
    strcpy(f.connection.label, "Must not persist");
    CHECK(UmiProviderConnectionsPut(f.store, &f.connection, true, 1U, &f.revision) == UMI_STATUS_INVALID_STATE);
    CHECK(f.revision == 1U);
    CHECK(UmiProviderConnectionsRead(f.store, &f.snapshot) == UMI_STATUS_INVALID_STATE);
    UmiProviderConnectionsDestroy(f.store); f.store = NULL;
    umi_data_server_destroy(f.server); f.server = NULL;
    OK(umi_data_server_create_sqlite("connections.sqlite3", &f.server));
    OK(umi_data_server_execute(f.server, "DROP TRIGGER lose_catalogue;"));
    OK(UmiProviderConnectionsOpen(f.server, "studio", "owner", &f.store));
    OK(UmiProviderConnectionsRead(f.store, &f.snapshot));
    CHECK(f.snapshot.count == 1U && f.snapshot.revision == 1U);
    CHECK(strcmp(f.snapshot.items[0].label, "Personal inference") == 0);
    return 0;
}
static int SeparateServer(void)
{
    UmiDataServer *other_server = NULL;
    OK(umi_data_server_create_sqlite("connections.sqlite3", &other_server));
    UmiStatus status = UmiProviderConnectionsOpen(other_server, "studio", "owner", &f.other);
    if (status == UMI_STATUS_OK) status = UmiProviderConnectionsRead(f.other, &f.snapshot);
    uint64_t reviewed = f.snapshot.revision;
    if (status == UMI_STATUS_OK) status = UmiProviderConnectionsPut(f.store, &f.connection, false, 0U, &f.revision);
    UmiStatus stale = status == UMI_STATUS_OK ?
        UmiProviderConnectionsPut(f.other, &f.connection, false, reviewed, &f.revision) : status;
    UmiProviderConnectionsDestroy(f.other); f.other = NULL;
    umi_data_server_destroy(other_server);
    OK(status);
    CHECK(stale == UMI_STATUS_BUSY && f.revision == 1U);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (strcmp(argv[1], "validation") == 0) return Validation();
    bool sqlite = strncmp(argv[1], "sqlite-", 7U) == 0;
    int result = Start(sqlite);
    if (result == 0) {
        if (strcmp(argv[1], "lifecycle") == 0) result = Lifecycle();
        else if (strcmp(argv[1], "stale") == 0) result = Stale();
        else if (strcmp(argv[1], "scope") == 0) result = Scope();
        else if (strcmp(argv[1], "limit") == 0) result = Limit();
        else if (strcmp(argv[1], "acquire") == 0) result = Acquire();
        else if (strcmp(argv[1], "failed-secret") == 0) result = FailedSecret();
        else if (strcmp(argv[1], "transaction") == 0) result = Transaction();
        else if (strcmp(argv[1], "corrupt") == 0) result = Corrupt();
        else if (strcmp(argv[1], "sqlite-reopen") == 0) result = Reopen();
        else if (strcmp(argv[1], "sqlite-failed-save") == 0) result = FailedSave(false);
        else if (strcmp(argv[1], "sqlite-failed-delete") == 0) result = FailedSave(true);
        else if (strcmp(argv[1], "boundaries") == 0) result = Boundaries();
        else if (strcmp(argv[1], "orphan") == 0) result = Orphan();
        else if (strcmp(argv[1], "sqlite-lost-transaction") == 0) result = LostTransaction();
        else if (strcmp(argv[1], "sqlite-separate-server") == 0) result = SeparateServer();
        else result = 2;
    }
    umi_secret_provider_registry_destroy(f.registry);
    UmiProviderConnectionsDestroy(f.other);
    UmiProviderConnectionsDestroy(f.store);
    umi_data_server_destroy(f.server);
    if (sqlite) (void)remove("connections.sqlite3");
    return result;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/data_server.c
 *
 * PURPOSE:
 *   Implement thread-safe memory and SQLite Data Server backends, transactional rollback, SQL execution, stable errors and authoritative record counting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/data_server.h"

#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef UMICOM_HAS_SQLITE
#include <sqlite3.h>
#endif

#define UMI_DATA_MAX_RECORDS 2048U
#define UMI_DATA_KEY_CAPACITY 192U
#define UMI_DATA_VALUE_CAPACITY 4096U
#define UMI_DATA_PATH_CAPACITY 1024U
#define UMI_DATA_ERROR_CAPACITY 512U

typedef struct UmiDataRecord {
    char key[UMI_DATA_KEY_CAPACITY];
    char value[UMI_DATA_VALUE_CAPACITY];
    int used;
} UmiDataRecord;

struct UmiDataServer {
    UmiDataServerBackend backend;
    atomic_flag lock;
    UmiDataRecord *records;
    UmiDataRecord *transaction_backup;
    size_t count;
    size_t transaction_backup_count;
    int transaction_active;
    char path[UMI_DATA_PATH_CAPACITY];
    char last_error[UMI_DATA_ERROR_CAPACITY];
#ifdef UMICOM_HAS_SQLITE
    sqlite3 *sqlite;
#endif
};

static void server_lock(UmiDataServer *server)
{
    while (atomic_flag_test_and_set_explicit(&server->lock,
                                              memory_order_acquire)) {
    }
}

static void server_unlock(UmiDataServer *server)
{
    atomic_flag_clear_explicit(&server->lock, memory_order_release);
}

#ifdef UMICOM_HAS_SQLITE
static void set_error(UmiDataServer *server, const char *message)
{
    if (server == NULL) return;
    (void)snprintf(server->last_error,
                   sizeof(server->last_error),
                   "%s",
                   message != NULL ? message : "");
}
#endif

static UmiStatus allocate_server(UmiDataServerBackend backend,
                                 UmiDataServer **out_server)
{
    UmiDataServer *server;
    if (out_server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_server = NULL;
    server = (UmiDataServer *)calloc(1U, sizeof(*server));
    if (server == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    server->backend = backend;
    atomic_flag_clear(&server->lock);
    if (backend == UMI_DATA_BACKEND_MEMORY) {
        server->records = (UmiDataRecord *)calloc(UMI_DATA_MAX_RECORDS,
                                                  sizeof(*server->records));
        server->transaction_backup =
            (UmiDataRecord *)calloc(UMI_DATA_MAX_RECORDS,
                                    sizeof(*server->transaction_backup));
        if (server->records == NULL || server->transaction_backup == NULL) {
            free(server->records);
            free(server->transaction_backup);
            free(server);
            return UMI_STATUS_OUT_OF_MEMORY;
        }
        (void)snprintf(server->path, sizeof(server->path), "%s", ":memory:");
    }
    *out_server = server;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_server_create_memory(UmiDataServer **out_server)
{
    return allocate_server(UMI_DATA_BACKEND_MEMORY, out_server);
}

UmiStatus umi_data_server_create_sqlite(const char *database_path,
                                        UmiDataServer **out_server)
{
#ifndef UMICOM_HAS_SQLITE
    (void)database_path;
    (void)out_server;
    return UMI_STATUS_UNAVAILABLE;
#else
    UmiDataServer *server;
    UmiStatus status;
    const char *schema =
        "CREATE TABLE IF NOT EXISTS umicom_kv ("
        " key TEXT PRIMARY KEY NOT NULL,"
        " value TEXT NOT NULL"
        ");";
    if (database_path == NULL || database_path[0] == '\0' ||
        out_server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = allocate_server(UMI_DATA_BACKEND_SQLITE, &server);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(server->path, sizeof(server->path), "%s", database_path);
    if (sqlite3_open_v2(database_path,
                        &server->sqlite,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                            SQLITE_OPEN_FULLMUTEX,
                        NULL) != SQLITE_OK) {
        set_error(server,
                  server->sqlite != NULL ? sqlite3_errmsg(server->sqlite) :
                  "SQLite open failed");
        if (server->sqlite != NULL) sqlite3_close(server->sqlite);
        free(server);
        return UMI_STATUS_IO_ERROR;
    }
    (void)sqlite3_busy_timeout(server->sqlite, 5000);
    if (sqlite3_exec(server->sqlite, schema, NULL, NULL, NULL) != SQLITE_OK) {
        set_error(server, sqlite3_errmsg(server->sqlite));
        sqlite3_close(server->sqlite);
        free(server);
        return UMI_STATUS_IO_ERROR;
    }
    *out_server = server;
    return UMI_STATUS_OK;
#endif
}

void umi_data_server_destroy(UmiDataServer *server)
{
    if (server == NULL) return;
#ifdef UMICOM_HAS_SQLITE
    if (server->sqlite != NULL) (void)sqlite3_close(server->sqlite);
#endif
    free(server->records);
    free(server->transaction_backup);
    free(server);
}

static UmiStatus memory_set(UmiDataServer *server,
                            const char *key,
                            const char *value)
{
    size_t index;
    UmiDataRecord *free_record = NULL;
    if (strlen(key) >= UMI_DATA_KEY_CAPACITY ||
        strlen(value) >= UMI_DATA_VALUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
        UmiDataRecord *record = &server->records[index];
        if (record->used && strcmp(record->key, key) == 0) {
            (void)snprintf(record->value, sizeof(record->value), "%s", value);
            return UMI_STATUS_OK;
        }
        if (!record->used && free_record == NULL) free_record = record;
    }
    if (free_record == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    free_record->used = 1;
    (void)snprintf(free_record->key, sizeof(free_record->key), "%s", key);
    (void)snprintf(free_record->value, sizeof(free_record->value), "%s", value);
    server->count++;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_server_set(UmiDataServer *server,
                              const char *key,
                              const char *value)
{
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    if (server == NULL || key == NULL || key[0] == '\0' || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    server_lock(server);
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        status = memory_set(server, key, value);
    }
#ifdef UMICOM_HAS_SQLITE
    else if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        const char *sql =
            "INSERT INTO umicom_kv(key,value) VALUES(?1,?2) "
            "ON CONFLICT(key) DO UPDATE SET value=excluded.value;";
        if (sqlite3_prepare(server->sqlite, sql, -1, &statement, NULL) !=
            SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } else {
            (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
            (void)sqlite3_bind_text(statement, 2, value, -1, SQLITE_TRANSIENT);
            status = sqlite3_step(statement) == SQLITE_DONE
                ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
            if (status != UMI_STATUS_OK) {
                set_error(server, sqlite3_errmsg(server->sqlite));
            }
            (void)sqlite3_finalize(statement);
        }
    }
#endif
    server_unlock(server);
    return status;
}

UmiStatus umi_data_server_get(const UmiDataServer *server_const,
                              const char *key,
                              char *value,
                              size_t value_capacity)
{
    UmiDataServer *server = (UmiDataServer *)server_const;
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    size_t index;
    if (server == NULL || key == NULL || value == NULL || value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    value[0] = '\0';
    server_lock(server);
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        status = UMI_STATUS_NOT_FOUND;
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            const UmiDataRecord *record = &server->records[index];
            if (record->used && strcmp(record->key, key) == 0) {
                if (strlen(record->value) + 1U > value_capacity) {
                    status = UMI_STATUS_CAPACITY_EXCEEDED;
                } else {
                    (void)snprintf(value, value_capacity, "%s", record->value);
                    status = UMI_STATUS_OK;
                }
                break;
            }
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        if (sqlite3_prepare(server->sqlite,
                               "SELECT value FROM umicom_kv WHERE key=?1;",
                               -1,
                               &statement,
                               NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } else {
            (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
            if (sqlite3_step(statement) == SQLITE_ROW) {
                const unsigned char *text = sqlite3_column_text(statement, 0);
                const char *source = text != NULL ? (const char *)text : "";
                status = strlen(source) + 1U > value_capacity
                    ? UMI_STATUS_CAPACITY_EXCEEDED
                    : UMI_STATUS_OK;
                if (status == UMI_STATUS_OK) {
                    (void)snprintf(value, value_capacity, "%s", source);
                }
            } else {
                status = UMI_STATUS_NOT_FOUND;
            }
            (void)sqlite3_finalize(statement);
        }
    }
#endif
    server_unlock(server);
    return status;
}

UmiStatus umi_data_server_delete(UmiDataServer *server, const char *key)
{
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    size_t index;
    if (server == NULL || key == NULL || key[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    server_lock(server);
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        status = UMI_STATUS_NOT_FOUND;
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            UmiDataRecord *record = &server->records[index];
            if (record->used && strcmp(record->key, key) == 0) {
                (void)memset(record, 0, sizeof(*record));
                server->count--;
                status = UMI_STATUS_OK;
                break;
            }
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        if (sqlite3_prepare(server->sqlite,
                               "DELETE FROM umicom_kv WHERE key=?1;",
                               -1,
                               &statement,
                               NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } else {
            (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
            status = sqlite3_step(statement) == SQLITE_DONE
                ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
            if (status == UMI_STATUS_OK && sqlite3_changes(server->sqlite) == 0) {
                status = UMI_STATUS_NOT_FOUND;
            }
            (void)sqlite3_finalize(statement);
        }
    }
#endif
    server_unlock(server);
    return status;
}

size_t umi_data_server_count(const UmiDataServer *server_const)
{
    UmiDataServer *server = (UmiDataServer *)server_const;
    size_t count = 0U;
    if (server == NULL) return 0U;
    server_lock(server);
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        count = server->count;
    }
#ifdef UMICOM_HAS_SQLITE
    else if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        if (sqlite3_prepare(server->sqlite,
                               "SELECT COUNT(*) FROM umicom_kv;",
                               -1,
                               &statement,
                               NULL) == SQLITE_OK &&
            sqlite3_step(statement) == SQLITE_ROW) {
            sqlite3_int64 value = sqlite3_column_int64(statement, 0);
            count = value > 0 ? (size_t)value : 0U;
        }
        if (statement != NULL) (void)sqlite3_finalize(statement);
    }
#endif
    server_unlock(server);
    return count;
}

UmiDataServerBackend umi_data_server_backend(const UmiDataServer *server)
{
    return server != NULL ? server->backend : UMI_DATA_BACKEND_MEMORY;
}

const char *umi_data_server_backend_name(const UmiDataServer *server)
{
    if (server == NULL) return "none";
    return server->backend == UMI_DATA_BACKEND_SQLITE ? "sqlite" : "memory";
}

UmiStatus umi_data_server_begin(UmiDataServer *server)
{
    UmiStatus status = UMI_STATUS_OK;
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    if (server->transaction_active) {
        server_unlock(server);
        return UMI_STATUS_BUSY;
    }
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        (void)memcpy(server->transaction_backup,
                     server->records,
                     UMI_DATA_MAX_RECORDS * sizeof(server->records[0]));
        server->transaction_backup_count = server->count;
    }
#ifdef UMICOM_HAS_SQLITE
    else if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        if (sqlite3_exec(server->sqlite,
                         "BEGIN IMMEDIATE;",
                         NULL,
                         NULL,
                         NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        }
    }
#endif
    if (status == UMI_STATUS_OK) server->transaction_active = 1;
    server_unlock(server);
    return status;
}

UmiStatus umi_data_server_commit(UmiDataServer *server)
{
    UmiStatus status = UMI_STATUS_OK;
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    if (!server->transaction_active) {
        server_unlock(server);
        return UMI_STATUS_INVALID_STATE;
    }
#ifdef UMICOM_HAS_SQLITE
    if (server->backend == UMI_DATA_BACKEND_SQLITE &&
        sqlite3_exec(server->sqlite, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK) {
        set_error(server, sqlite3_errmsg(server->sqlite));
        status = UMI_STATUS_IO_ERROR;
    }
#endif
    if (status == UMI_STATUS_OK) {
        server->transaction_active = 0;
        server->transaction_backup_count = 0U;
    }
    server_unlock(server);
    return status;
}

UmiStatus umi_data_server_rollback(UmiDataServer *server)
{
    UmiStatus status = UMI_STATUS_OK;
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    if (!server->transaction_active) {
        server_unlock(server);
        return UMI_STATUS_INVALID_STATE;
    }
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        (void)memcpy(server->records,
                     server->transaction_backup,
                     UMI_DATA_MAX_RECORDS * sizeof(server->records[0]));
        server->count = server->transaction_backup_count;
    }
#ifdef UMICOM_HAS_SQLITE
    else if (server->backend == UMI_DATA_BACKEND_SQLITE &&
             sqlite3_exec(server->sqlite,
                          "ROLLBACK;",
                          NULL,
                          NULL,
                          NULL) != SQLITE_OK) {
        set_error(server, sqlite3_errmsg(server->sqlite));
        status = UMI_STATUS_IO_ERROR;
    }
#endif
    server->transaction_active = 0;
    server->transaction_backup_count = 0U;
    server_unlock(server);
    return status;
}

int umi_data_server_in_transaction(const UmiDataServer *server)
{
    return server != NULL ? server->transaction_active : 0;
}

UmiStatus umi_data_server_execute(UmiDataServer *server, const char *sql)
{
    if (server == NULL || sql == NULL || sql[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifndef UMICOM_HAS_SQLITE
    (void)server;
    (void)sql;
    return UMI_STATUS_UNAVAILABLE;
#else
    if (server->backend != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    server_lock(server);
    if (sqlite3_exec(server->sqlite, sql, NULL, NULL, NULL) != SQLITE_OK) {
        set_error(server, sqlite3_errmsg(server->sqlite));
        server_unlock(server);
        return UMI_STATUS_IO_ERROR;
    }
    server_unlock(server);
    return UMI_STATUS_OK;
#endif
}

const char *umi_data_server_path(const UmiDataServer *server)
{
    return server != NULL ? server->path : "";
}

const char *umi_data_server_last_error(const UmiDataServer *server)
{
    return server != NULL ? server->last_error : "";
}

UmiStatus umi_data_server_visit(const UmiDataServer *server_const,
                                UmiDataServerRecordVisitor visitor,
                                void *user_data)
{
    UmiDataServer *server = (UmiDataServer *)server_const;
    UmiStatus status = UMI_STATUS_OK;
    size_t index;
    if (server == NULL || visitor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            if (!server->records[index].used) continue;
            status = visitor(server->records[index].key,
                             server->records[index].value,
                             user_data);
            if (status != UMI_STATUS_OK) break;
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        if (sqlite3_prepare(server->sqlite,
                               "SELECT key,value FROM umicom_kv ORDER BY key;",
                               -1, &statement, NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } else {
            while (sqlite3_step(statement) == SQLITE_ROW) {
                const unsigned char *key = sqlite3_column_text(statement, 0);
                const unsigned char *value = sqlite3_column_text(statement, 1);
                status = visitor((const char *)(key != NULL ? key : (const unsigned char *)""),
                                 (const char *)(value != NULL ? value : (const unsigned char *)""),
                                 user_data);
                if (status != UMI_STATUS_OK) break;
            }
            (void)sqlite3_finalize(statement);
        }
    }
#endif
    server_unlock(server);
    return status;
}

UmiStatus umi_data_server_snapshot(const UmiDataServer *server,
                                   UmiDataServerSnapshot *out_snapshot)
{
    int written;
    if (server == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->backend = umi_data_server_backend(server);
    out_snapshot->record_count = umi_data_server_count(server);
    out_snapshot->transaction_active = umi_data_server_in_transaction(server);
    written = snprintf(out_snapshot->backend_name,
                       sizeof(out_snapshot->backend_name), "%s",
                       umi_data_server_backend_name(server));
    if (written < 0 || (size_t)written >= sizeof(out_snapshot->backend_name)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(out_snapshot->path, sizeof(out_snapshot->path), "%s",
                       umi_data_server_path(server));
    if (written < 0 || (size_t)written >= sizeof(out_snapshot->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(out_snapshot->last_error,
                       sizeof(out_snapshot->last_error), "%s",
                       umi_data_server_last_error(server));
    return written < 0 || (size_t)written >= sizeof(out_snapshot->last_error)
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}

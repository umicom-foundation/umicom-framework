/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/workspace/framework/src/data/data_server.c
 *
 * PURPOSE:
 *   Implement the data server behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/data_server.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef UMICOM_HAS_SQLITE
#include <sqlite3.h>
#endif

#define UMI_DATA_MAX_RECORDS 256U

typedef struct UmiDataRecord {
    char key[128];
    char value[1024];
    int used;
} UmiDataRecord;

struct UmiDataServer {
    UmiDataServerBackend backend;
    UmiDataRecord records[UMI_DATA_MAX_RECORDS];
    size_t count;
#ifdef UMICOM_HAS_SQLITE
    sqlite3 *sqlite;
#endif
};

UmiStatus umi_data_server_create_memory(UmiDataServer **out_server)
{
    UmiDataServer *server;
    if (out_server == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_server = 0;
    server = (UmiDataServer *)calloc(1U, sizeof(*server));
    if (server == 0) return UMI_STATUS_OUT_OF_MEMORY;
    server->backend = UMI_DATA_BACKEND_MEMORY;
    *out_server = server;
    return UMI_STATUS_OK;
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
    const char *schema =
        "CREATE TABLE IF NOT EXISTS umicom_kv ("
        " key TEXT PRIMARY KEY NOT NULL,"
        " value TEXT NOT NULL"
        ");";
    if (database_path == 0 || out_server == 0) return UMI_STATUS_INVALID_ARGUMENT;
    *out_server = 0;
    server = (UmiDataServer *)calloc(1U, sizeof(*server));
    if (server == 0) return UMI_STATUS_OUT_OF_MEMORY;
    server->backend = UMI_DATA_BACKEND_SQLITE;
    if (sqlite3_open(database_path, &server->sqlite) != SQLITE_OK) {
        if (server->sqlite != 0) sqlite3_close(server->sqlite);
        free(server);
        return UMI_STATUS_IO_ERROR;
    }
    if (sqlite3_exec(server->sqlite, schema, 0, 0, 0) != SQLITE_OK) {
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
    if (server == 0) return;
#ifdef UMICOM_HAS_SQLITE
    if (server->backend == UMI_DATA_BACKEND_SQLITE && server->sqlite != 0)
        sqlite3_close(server->sqlite);
#endif
    free(server);
}

static UmiStatus memory_set(UmiDataServer *server, const char *key, const char *value)
{
    size_t index;
    UmiDataRecord *free_record = 0;
    for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
        UmiDataRecord *record = &server->records[index];
        if (record->used && strcmp(record->key, key) == 0) {
            (void)snprintf(record->value, sizeof(record->value), "%s", value);
            return UMI_STATUS_OK;
        }
        if (!record->used && free_record == 0) free_record = record;
    }
    if (free_record == 0) return UMI_STATUS_CAPACITY_EXCEEDED;
    free_record->used = 1;
    (void)snprintf(free_record->key, sizeof(free_record->key), "%s", key);
    (void)snprintf(free_record->value, sizeof(free_record->value), "%s", value);
    server->count++;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_server_set(UmiDataServer *server, const char *key,
                              const char *value)
{
    if (server == 0 || key == 0 || key[0] == '\0' || value == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (server->backend == UMI_DATA_BACKEND_MEMORY) return memory_set(server, key, value);
#ifdef UMICOM_HAS_SQLITE
    if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = 0;
        const char *sql =
            "INSERT INTO umicom_kv(key,value) VALUES(?1,?2) "
            "ON CONFLICT(key) DO UPDATE SET value=excluded.value;";
        if (sqlite3_prepare(server->sqlite, sql, -1, &statement, 0) != SQLITE_OK)
            return UMI_STATUS_IO_ERROR;
        (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
        (void)sqlite3_bind_text(statement, 2, value, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(statement) != SQLITE_DONE) {
            sqlite3_finalize(statement);
            return UMI_STATUS_IO_ERROR;
        }
        sqlite3_finalize(statement);
        return UMI_STATUS_OK;
    }
#endif
    return UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_data_server_get(const UmiDataServer *server, const char *key,
                              char *value, size_t value_capacity)
{
    size_t index;
    if (server == 0 || key == 0 || value == 0 || value_capacity == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    value[0] = '\0';
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            const UmiDataRecord *record = &server->records[index];
            if (record->used && strcmp(record->key, key) == 0) {
                (void)snprintf(value, value_capacity, "%s", record->value);
                return UMI_STATUS_OK;
            }
        }
        return UMI_STATUS_NOT_FOUND;
    }
#ifdef UMICOM_HAS_SQLITE
    if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = 0;
        const unsigned char *text;
        if (sqlite3_prepare(server->sqlite,
                               "SELECT value FROM umicom_kv WHERE key=?1;",
                               -1, &statement, 0) != SQLITE_OK)
            return UMI_STATUS_IO_ERROR;
        (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(statement) != SQLITE_ROW) {
            sqlite3_finalize(statement);
            return UMI_STATUS_NOT_FOUND;
        }
        text = sqlite3_column_text(statement, 0);
        (void)snprintf(value, value_capacity, "%s", text != 0 ? (const char *)text : "");
        sqlite3_finalize(statement);
        return UMI_STATUS_OK;
    }
#endif
    return UMI_STATUS_INVALID_STATE;
}

UmiStatus umi_data_server_delete(UmiDataServer *server, const char *key)
{
    size_t index;
    if (server == 0 || key == 0) return UMI_STATUS_INVALID_ARGUMENT;
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            UmiDataRecord *record = &server->records[index];
            if (record->used && strcmp(record->key, key) == 0) {
                memset(record, 0, sizeof(*record));
                server->count--;
                return UMI_STATUS_OK;
            }
        }
        return UMI_STATUS_NOT_FOUND;
    }
#ifdef UMICOM_HAS_SQLITE
    if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = 0;
        if (sqlite3_prepare(server->sqlite,
                               "DELETE FROM umicom_kv WHERE key=?1;",
                               -1, &statement, 0) != SQLITE_OK)
            return UMI_STATUS_IO_ERROR;
        (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
        if (sqlite3_step(statement) != SQLITE_DONE) {
            sqlite3_finalize(statement);
            return UMI_STATUS_IO_ERROR;
        }
        sqlite3_finalize(statement);
        return sqlite3_changes(server->sqlite) > 0 ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
    }
#endif
    return UMI_STATUS_INVALID_STATE;
}

size_t umi_data_server_count(const UmiDataServer *server)
{
    if (server == 0) return 0U;
    if (server->backend == UMI_DATA_BACKEND_MEMORY) return server->count;
#ifdef UMICOM_HAS_SQLITE
    if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = 0;
        size_t count = 0U;
        if (sqlite3_prepare(server->sqlite,
                               "SELECT COUNT(*) FROM umicom_kv;",
                               -1, &statement, 0) == SQLITE_OK &&
            sqlite3_step(statement) == SQLITE_ROW)
            count = (size_t)sqlite3_column_int64(statement, 0);
        if (statement != 0) sqlite3_finalize(statement);
        return count;
    }
#endif
    return 0U;
}

UmiDataServerBackend umi_data_server_backend(const UmiDataServer *server)
{
    return server != 0 ? server->backend : UMI_DATA_BACKEND_MEMORY;
}

const char *umi_data_server_backend_name(const UmiDataServer *server)
{
    if (server == 0) return "none";
    return server->backend == UMI_DATA_BACKEND_SQLITE ? "sqlite" : "memory";
}

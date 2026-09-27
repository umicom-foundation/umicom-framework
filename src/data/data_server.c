/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/data_server.c
 *
 * PURPOSE:
 *   Implement thread-safe memory and SQLite Data Server backends, transactional rollback, SQL execution, stable errors and authoritative record counting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/data_server.h"

#include <stdatomic.h>
#include <stdint.h>
#include <limits.h>
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
    /* Transaction responsibility spans individual calls. The non-reused token
     * avoids accepting a different thread after native thread IDs are recycled. */
    uint_fast64_t transaction_owner;
    int transaction_lost;
    int rollback_failed;
    char path[UMI_DATA_PATH_CAPACITY];
    char last_error[UMI_DATA_ERROR_CAPACITY];
#ifdef UMICOM_HAS_SQLITE
    sqlite3 *sqlite;
#endif
};

/* Provide the server lock operation used by this module and its client applications. */
static void server_lock(UmiDataServer *server)
{
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (atomic_flag_test_and_set_explicit(&server->lock,
                                              memory_order_acquire)) {
    }
}

/* Provide the server unlock operation used by this module and its client applications. */
static void server_unlock(UmiDataServer *server)
{
    atomic_flag_clear_explicit(&server->lock, memory_order_release);
}


/* One token per calling thread, allocated without an OS-specific public ABI.
 * Exhaustion is refused permanently; the counter never wraps and tokens are
 * never recycled when a worker exits. This does not manage worker lifetimes. */
static atomic_uint_fast64_t DataNextThread = 0;
static _Thread_local uint_fast64_t DataLocalThread;
static uint_fast64_t DataThreadToken(void)
{
    if (DataLocalThread != 0) return DataLocalThread;
    uint_fast64_t seen = atomic_load_explicit(&DataNextThread, memory_order_relaxed);
    while (seen != UINT_FAST64_MAX) {
        if (atomic_compare_exchange_weak_explicit(&DataNextThread, &seen, seen + 1U,
                memory_order_relaxed, memory_order_relaxed)) {
            DataLocalThread = seen + 1U;
            return DataLocalThread;
        }
    }
    return 0;
}

typedef struct DataVisitFrame {
    const UmiDataServer *server;
    struct DataVisitFrame *previous;
} DataVisitFrame;
static _Thread_local DataVisitFrame *DataVisits;
static int DataInVisitor(const UmiDataServer *server)
{
    for (DataVisitFrame *f = DataVisits; f != NULL; f = f->previous)
        if (f->server == server) return 1;
    return 0;
}
static void DataMessage(UmiDataServer *server, const char *text)
{
    (void)snprintf(server->last_error, sizeof server->last_error, "%s", text);
}
/* Success leaves the existing server lock held. A competing transaction user
 * returns BUSY instead of taking part in another worker's pending changes.
 * Visitors still receive borrowed stable strings under the lock; direct
 * re-entry is refused before attempting that non-recursive lock. */
static UmiStatus DataEnter(UmiDataServer *server, int recovery)
{
    if (DataInVisitor(server)) return UMI_STATUS_BUSY;
    uint_fast64_t token = DataThreadToken();
    if (token == 0) return UMI_STATUS_CAPACITY_EXCEEDED;
    server_lock(server);
    if (server->transaction_owner != 0 && server->transaction_owner != token) {
        server_unlock(server);
        return UMI_STATUS_BUSY;
    }
    if (!recovery && (server->rollback_failed || server->transaction_lost)) {
        server_unlock(server);
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
static void DataTransactionClear(UmiDataServer *server)
{
    server->transaction_active = 0;
    server->transaction_backup_count = 0;
    server->transaction_owner = 0;
    server->transaction_lost = 0;
    server->rollback_failed = 0;
}
#ifdef UMICOM_HAS_SQLITE
static UmiStatus DataSqlError(UmiDataServer *server, int code)
{
    DataMessage(server, sqlite3_errmsg(server->sqlite));
    if ((code & 0xff) == SQLITE_TOOBIG) return UMI_STATUS_CAPACITY_EXCEEDED;
    if ((code & 0xff) == SQLITE_NOMEM) return UMI_STATUS_OUT_OF_MEMORY;
    return UMI_STATUS_IO_ERROR;
}
/* SQLite may abort the entire transaction after a failed statement. Record
 * that observation, but keep its owner until explicit rollback acknowledges
 * the loss. No subsequent statement can silently run in autocommit mode. */
static void DataObserveAbort(UmiDataServer *server)
{
    if (server->backend == UMI_DATA_BACKEND_SQLITE && server->transaction_active &&
        sqlite3_get_autocommit(server->sqlite)) {
        server->transaction_active = 0;
        server->transaction_lost = 1;
    }
}
static UmiStatus DataCountLocked(UmiDataServer *server, size_t *count)
{
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        *count = server->count;
        return UMI_STATUS_OK;
    }
    sqlite3_stmt *statement = NULL;
    int code = sqlite3_prepare_v2(server->sqlite, "SELECT COUNT(*) FROM umicom_kv;",
        -1, &statement, NULL);
    if (code == SQLITE_OK) code = sqlite3_step(statement);
    UmiStatus result = UMI_STATUS_OK;
    if (code == SQLITE_ROW) {
        sqlite3_int64 value = sqlite3_column_int64(statement, 0);
        if (value < 0 || (uint64_t)value > SIZE_MAX) result = UMI_STATUS_CAPACITY_EXCEEDED;
        else *count = (size_t)value;
    } else result = DataSqlError(server, code);
    if (statement != NULL) {
        code = sqlite3_finalize(statement);
        if (result == UMI_STATUS_OK && code != SQLITE_OK) result = DataSqlError(server, code);
    }
    return result;
}
#else
static void DataObserveAbort(UmiDataServer *server) { (void)server; }
static UmiStatus DataCountLocked(UmiDataServer *server, size_t *count)
{
    *count = server->count;
    return UMI_STATUS_OK;
}
#endif

#ifdef UMICOM_HAS_SQLITE
/* Provide the set error operation used by this module and its client applications. */
static void set_error(UmiDataServer *server, const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return;
    (void)snprintf(server->last_error,
                   sizeof(server->last_error),
                   "%s",
                   message != NULL ? message : "");
}
#endif

/* Provide the allocate server operation used by this module and its client applications. */
static UmiStatus allocate_server(UmiDataServerBackend backend,
                                 UmiDataServer **out_server)
{
    UmiDataServer *server;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_server = NULL;
    server = (UmiDataServer *)calloc(1U, sizeof(*server));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    server->backend = backend;
    atomic_flag_clear(&server->lock);
    /* Apply this branch only when its contract condition is satisfied. */
    if (backend == UMI_DATA_BACKEND_MEMORY) {
        server->records = (UmiDataRecord *)calloc(UMI_DATA_MAX_RECORDS,
                                                  sizeof(*server->records));
        server->transaction_backup =
            (UmiDataRecord *)calloc(UMI_DATA_MAX_RECORDS,
                                    sizeof(*server->transaction_backup));
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
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

/*
 * Provide the data server create memory operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_server_create_memory(UmiDataServer **out_server)
{
    return allocate_server(UMI_DATA_BACKEND_MEMORY, out_server);
}

/*
 * Provide the data server create sqlite operation used by this module and its client
 * applications.
 */
/* Superseded implementation retained for engineering review.
 * Creation must clear its output on failure and must not record a truncated database path.
 * The active implementation below keeps the public entry point. */
#if 0
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database_path == NULL || database_path[0] == '\0' ||
        out_server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = allocate_server(UMI_DATA_BACKEND_SQLITE, &server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(server->path, sizeof(server->path), "%s", database_path);
    /* Apply this branch only when its contract condition is satisfied. */
    if (sqlite3_open_v2(database_path,
                        &server->sqlite,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                            SQLITE_OPEN_FULLMUTEX,
                        NULL) != SQLITE_OK) {
        set_error(server,
                  server->sqlite != NULL ? sqlite3_errmsg(server->sqlite) :
                  "SQLite open failed");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (server->sqlite != NULL) sqlite3_close(server->sqlite);
        free(server);
        return UMI_STATUS_IO_ERROR;
    }
    (void)sqlite3_busy_timeout(server->sqlite, 5000);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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
#endif

UmiStatus umi_data_server_create_sqlite(const char *database_path,
                                        UmiDataServer **out_server)
{
    if (out_server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_server = NULL;
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (database_path == NULL || database_path[0] == '\0' ||
        out_server == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strlen(database_path) >= UMI_DATA_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = allocate_server(UMI_DATA_BACKEND_SQLITE, &server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(server->path, sizeof(server->path), "%s", database_path);
    /* Apply this branch only when its contract condition is satisfied. */
    if (sqlite3_open_v2(database_path,
                        &server->sqlite,
                        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE |
                            SQLITE_OPEN_FULLMUTEX,
                        NULL) != SQLITE_OK) {
        set_error(server,
                  server->sqlite != NULL ? sqlite3_errmsg(server->sqlite) :
                  "SQLite open failed");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (server->sqlite != NULL) sqlite3_close(server->sqlite);
        free(server);
        return UMI_STATUS_IO_ERROR;
    }
    (void)sqlite3_busy_timeout(server->sqlite, 5000);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/* Release or reset state held by data server so the same storage can be reused safely. */
void umi_data_server_destroy(UmiDataServer *server)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return;
#ifdef UMICOM_HAS_SQLITE
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server->sqlite != NULL) (void)sqlite3_close(server->sqlite);
#endif
    free(server->records);
    free(server->transaction_backup);
    free(server);
}

/* Copy memory into module-owned storage so callers keep ownership of their input values. */
static UmiStatus memory_set(UmiDataServer *server,
                            const char *key,
                            const char *value)
{
    size_t index;
    UmiDataRecord *free_record = NULL;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(key) >= UMI_DATA_KEY_CAPACITY ||
        strlen(value) >= UMI_DATA_VALUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
        UmiDataRecord *record = &server->records[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (record->used && strcmp(record->key, key) == 0) {
            (void)snprintf(record->value, sizeof(record->value), "%s", value);
            return UMI_STATUS_OK;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (!record->used && free_record == NULL) free_record = record;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (free_record == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    free_record->used = 1;
    (void)snprintf(free_record->key, sizeof(free_record->key), "%s", key);
    (void)snprintf(free_record->value, sizeof(free_record->value), "%s", value);
    server->count++;
    return UMI_STATUS_OK;
}

/*
 * Copy data server into module-owned storage so callers keep ownership of their input
 * values.
 */
/* Superseded implementation retained for engineering review.
 * Individual-call locking did not reserve a transaction for its initiating worker, and SQLite binding failures were ignored.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_set(UmiDataServer *server,
                              const char *key,
                              const char *value)
{
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || key == NULL || key[0] == '\0' || value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    server_lock(server);
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        status = memory_set(server, key, value);
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        const char *sql =
            "INSERT INTO umicom_kv(key,value) VALUES(?1,?2) "
            "ON CONFLICT(key) DO UPDATE SET value=excluded.value;";
        /* Apply this branch only when its contract condition is satisfied. */
        if (sqlite3_prepare(server->sqlite, sql, -1, &statement, NULL) !=
            SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
            (void)sqlite3_bind_text(statement, 2, value, -1, SQLITE_TRANSIENT);
            status = sqlite3_step(statement) == SQLITE_DONE
                ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
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
#endif

UmiStatus umi_data_server_set(UmiDataServer *server, const char *key, const char *value)
{
    if (server == NULL || key == NULL || key[0] == '\0' || value == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    if (server->backend == UMI_DATA_BACKEND_MEMORY) status = memory_set(server, key, value);
#ifdef UMICOM_HAS_SQLITE
    else {
        sqlite3_stmt *statement = NULL;
        int code = sqlite3_prepare_v2(server->sqlite,
            "INSERT INTO umicom_kv(key,value) VALUES(?1,?2) "
            "ON CONFLICT(key) DO UPDATE SET value=excluded.value;", -1, &statement, NULL);
        if (code == SQLITE_OK) code = sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
        if (code == SQLITE_OK) code = sqlite3_bind_text(statement, 2, value, -1, SQLITE_TRANSIENT);
        if (code == SQLITE_OK) code = sqlite3_step(statement);
        status = code == SQLITE_DONE ? UMI_STATUS_OK : DataSqlError(server, code);
        if (statement != NULL) {
            code = sqlite3_finalize(statement);
            if (status == UMI_STATUS_OK && code != SQLITE_OK) status = DataSqlError(server, code);
        }
        DataObserveAbort(server);
    }
#endif
    server_unlock(server);
    return status;
}

/* Provide the data server get operation used by this module and its client applications. */
/* Superseded implementation retained for engineering review.
 * Reads must respect transaction ownership and distinguish a failed bind from a genuinely absent key.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_get(const UmiDataServer *server_const,
                              const char *key,
                              char *value,
                              size_t value_capacity)
{
    UmiDataServer *server = (UmiDataServer *)server_const;
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || key == NULL || value == NULL || value_capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    value[0] = '\0';
    server_lock(server);
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        status = UMI_STATUS_NOT_FOUND;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            const UmiDataRecord *record = &server->records[index];
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (record->used && strcmp(record->key, key) == 0) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (strlen(record->value) + 1U > value_capacity) {
                    status = UMI_STATUS_CAPACITY_EXCEEDED;
                } /* Use this fallback path when the earlier condition does not apply. */ else {
                    /* The preceding capacity check and this server lock already
                     * prove the stored string fits. A direct byte copy makes
                     * that contract explicit without a redundant printf format.
                     * Keep the original operation below for engineering review. */
#if 0
                    (void)snprintf(value, value_capacity, "%s", record->value);
#endif
                    (void)memcpy(value, record->value, strlen(record->value) + 1U);
                    status = UMI_STATUS_OK;
                }
                break;
            }
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        /* Apply this branch only when its contract condition is satisfied. */
        if (sqlite3_prepare(server->sqlite,
                               "SELECT value FROM umicom_kv WHERE key=?1;",
                               -1,
                               &statement,
                               NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
            /* Distinguish exhausted input from a SQLite read failure. Banking
             * recovery and every other repository must not interpret IO/step
             * errors as an absent record. This replaces the branch below;
             * its implementation is retained unchanged for engineering review. */
#if 0
            /* Apply this branch only when its contract condition is satisfied. */
            if (sqlite3_step(statement) == SQLITE_ROW) {
                const unsigned char *text = sqlite3_column_text(statement, 0);
                const char *source = text != NULL ? (const char *)text : "";
                status = strlen(source) + 1U > value_capacity
                    ? UMI_STATUS_CAPACITY_EXCEEDED
                    : UMI_STATUS_OK;
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status == UMI_STATUS_OK) {
                    (void)snprintf(value, value_capacity, "%s", source);
                }
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                status = UMI_STATUS_NOT_FOUND;
            }
#endif
            /* Data Server remains the authority for storage error semantics.
             * Reject embedded NUL data rather than silently truncating a stored
             * value to a different, apparently valid C string. */
            {
                int step_result = sqlite3_step(statement);
                if (step_result == SQLITE_ROW) {
                    const unsigned char *text = sqlite3_column_text(statement, 0);
                    int bytes = sqlite3_column_bytes(statement, 0);
                    if (text == NULL || bytes < 0) {
                        set_error(server, "SQLite text conversion failed");
                        status = UMI_STATUS_IO_ERROR;
                    } else if (memchr(text, 0, (size_t)bytes) != NULL) {
                        set_error(server, "Stored text contains an embedded NUL");
                        status = UMI_STATUS_PARSE_ERROR;
                    } else if ((size_t)bytes >= value_capacity) {
                        status = UMI_STATUS_CAPACITY_EXCEEDED;
                    } else {
                        memcpy(value, text, (size_t)bytes);
                        value[(size_t)bytes] = '\0';
                        status = UMI_STATUS_OK;
                    }
                } else if (step_result == SQLITE_DONE) {
                    status = UMI_STATUS_NOT_FOUND;
                } else {
                    set_error(server, sqlite3_errmsg(server->sqlite));
                    status = UMI_STATUS_IO_ERROR;
                }
            }
            (void)sqlite3_finalize(statement);
        }
    }
#endif
    server_unlock(server);
    return status;
}
#endif

UmiStatus umi_data_server_get(const UmiDataServer *source, const char *key,
    char *value, size_t capacity)
{
    if (value != NULL && capacity != 0) value[0] = '\0';
    if (source == NULL || key == NULL || value == NULL || capacity == 0)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDataServer *server = (UmiDataServer *)source;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    status = UMI_STATUS_NOT_FOUND;
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        for (size_t i = 0; i < UMI_DATA_MAX_RECORDS; ++i) {
            const UmiDataRecord *record = &server->records[i];
            if (!record->used || strcmp(record->key, key) != 0) continue;
            size_t length = strlen(record->value);
            if (length >= capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
            else { memcpy(value, record->value, length + 1U); status = UMI_STATUS_OK; }
            break;
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else {
        sqlite3_stmt *statement = NULL;
        int code = sqlite3_prepare_v2(server->sqlite,
            "SELECT value FROM umicom_kv WHERE key=?1;", -1, &statement, NULL);
        if (code == SQLITE_OK) code = sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
        if (code == SQLITE_OK) code = sqlite3_step(statement);
        if (code == SQLITE_ROW) {
            const unsigned char *text = sqlite3_column_text(statement, 0);
            int bytes = sqlite3_column_bytes(statement, 0);
            if (text == NULL || bytes < 0) status = UMI_STATUS_IO_ERROR;
            else if (memchr(text, 0, (size_t)bytes) != NULL) status = UMI_STATUS_PARSE_ERROR;
            else if ((size_t)bytes >= capacity) status = UMI_STATUS_CAPACITY_EXCEEDED;
            else { memcpy(value, text, (size_t)bytes); value[bytes] = '\0'; status = UMI_STATUS_OK; }
        } else if (code != SQLITE_DONE) status = DataSqlError(server, code);
        if (statement != NULL) {
            code = sqlite3_finalize(statement);
            if (status == UMI_STATUS_OK && code != SQLITE_OK) status = DataSqlError(server, code);
        }
        DataObserveAbort(server);
    }
#endif
    if (status != UMI_STATUS_OK) value[0] = '\0';
    server_unlock(server);
    return status;
}

/*
 * Provide the data server delete operation used by this module and its client
 * applications.
 */
/* Superseded implementation retained for engineering review.
 * Deletion must not join another worker transaction or ignore SQLite binding failures.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_delete(UmiDataServer *server, const char *key)
{
    UmiStatus status = UMI_STATUS_INVALID_STATE;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || key == NULL || key[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    server_lock(server);
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        status = UMI_STATUS_NOT_FOUND;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            UmiDataRecord *record = &server->records[index];
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (record->used && strcmp(record->key, key) == 0) {
                (void)memset(record, 0, sizeof(*record));
                server->count--;
                status = UMI_STATUS_OK;
                break;
            }
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        /* Apply this branch only when its contract condition is satisfied. */
        if (sqlite3_prepare(server->sqlite,
                               "DELETE FROM umicom_kv WHERE key=?1;",
                               -1,
                               &statement,
                               NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            (void)sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
            status = sqlite3_step(statement) == SQLITE_DONE
                ? UMI_STATUS_OK : UMI_STATUS_IO_ERROR;
            /* Preserve the original failure result so the caller can respond to the correct cause. */
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
#endif

UmiStatus umi_data_server_delete(UmiDataServer *server, const char *key)
{
    if (server == NULL || key == NULL || key[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    status = UMI_STATUS_NOT_FOUND;
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        for (size_t i = 0; i < UMI_DATA_MAX_RECORDS; ++i) {
            UmiDataRecord *record = &server->records[i];
            if (record->used && strcmp(record->key, key) == 0) {
                memset(record, 0, sizeof *record); --server->count; status = UMI_STATUS_OK; break;
            }
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else {
        sqlite3_stmt *statement = NULL;
        int code = sqlite3_prepare_v2(server->sqlite,
            "DELETE FROM umicom_kv WHERE key=?1;", -1, &statement, NULL);
        if (code == SQLITE_OK) code = sqlite3_bind_text(statement, 1, key, -1, SQLITE_TRANSIENT);
        if (code == SQLITE_OK) code = sqlite3_step(statement);
        status = code == SQLITE_DONE ? (sqlite3_changes(server->sqlite) ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND)
            : DataSqlError(server, code);
        if (statement != NULL) {
            code = sqlite3_finalize(statement);
            if (status == UMI_STATUS_OK && code != SQLITE_OK) status = DataSqlError(server, code);
        }
        DataObserveAbort(server);
    }
#endif
    server_unlock(server);
    return status;
}

/* Return the number of records represented by data server without changing their state. */
/* Superseded implementation retained for engineering review.
 * The legacy scalar result cannot represent a count failure. It now delegates to the additive checked interface; failure remains zero for compatibility.
 * The active implementation below keeps the public entry point. */
#if 0
size_t umi_data_server_count(const UmiDataServer *server_const)
{
    UmiDataServer *server = (UmiDataServer *)server_const;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return 0U;
    server_lock(server);
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        count = server->count;
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        /* Apply this branch only when its contract condition is satisfied. */
        if (sqlite3_prepare(server->sqlite,
                               "SELECT COUNT(*) FROM umicom_kv;",
                               -1,
                               &statement,
                               NULL) == SQLITE_OK &&
            sqlite3_step(statement) == SQLITE_ROW) {
            sqlite3_int64 value = sqlite3_column_int64(statement, 0);
            count = value > 0 ? (size_t)value : 0U;
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (statement != NULL) (void)sqlite3_finalize(statement);
    }
#endif
    server_unlock(server);
    return count;
}
#endif

size_t umi_data_server_count(const UmiDataServer *server)
{
    size_t result = 0;
    (void)UmiDataServerCountChecked(server, &result);
    return result;
}

/*
 * Provide the data server backend operation used by this module and its client
 * applications.
 */
UmiDataServerBackend umi_data_server_backend(const UmiDataServer *server)
{
    return server != NULL ? server->backend : UMI_DATA_BACKEND_MEMORY;
}

/*
 * Provide the data server backend name operation used by this module and its client
 * applications.
 */
const char *umi_data_server_backend_name(const UmiDataServer *server)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return "none";
    return server->backend == UMI_DATA_BACKEND_SQLITE ? "sqlite" : "memory";
}

/* Provide the data server begin operation used by this module and its client applications. */
/* Superseded implementation retained for engineering review.
 * Transaction ownership must remain reserved after the begin call releases the internal lock.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_begin(UmiDataServer *server)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    /* Apply this operation only while the related capability or state is available. */
    if (server->transaction_active) {
        server_unlock(server);
        return UMI_STATUS_BUSY;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        (void)memcpy(server->transaction_backup,
                     server->records,
                     UMI_DATA_MAX_RECORDS * sizeof(server->records[0]));
        server->transaction_backup_count = server->count;
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) server->transaction_active = 1;
    server_unlock(server);
    return status;
}
#endif

UmiStatus umi_data_server_begin(UmiDataServer *server)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    if (server->transaction_active) { server_unlock(server); return UMI_STATUS_BUSY; }
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        memcpy(server->transaction_backup, server->records,
            UMI_DATA_MAX_RECORDS * sizeof server->records[0]);
        server->transaction_backup_count = server->count;
    }
#ifdef UMICOM_HAS_SQLITE
    else {
        int code = sqlite3_exec(server->sqlite, "BEGIN IMMEDIATE;", NULL, NULL, NULL);
        if (code != SQLITE_OK) status = DataSqlError(server, code);
    }
#endif
    if (status == UMI_STATUS_OK) {
        server->transaction_active = 1;
        server->transaction_owner = DataThreadToken();
    }
    server_unlock(server);
    return status;
}

/*
 * Provide the data server commit operation used by this module and its client
 * applications.
 */
/* Superseded implementation retained for engineering review.
 * Only the initiating worker may commit. An unsuccessful commit retains backend-observed recovery responsibility.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_commit(UmiDataServer *server)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    /* Apply this operation only while the related capability or state is available. */
    if (!server->transaction_active) {
        server_unlock(server);
        return UMI_STATUS_INVALID_STATE;
    }
#ifdef UMICOM_HAS_SQLITE
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_SQLITE &&
        sqlite3_exec(server->sqlite, "COMMIT;", NULL, NULL, NULL) != SQLITE_OK) {
        set_error(server, sqlite3_errmsg(server->sqlite));
        status = UMI_STATUS_IO_ERROR;
    }
#endif
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        server->transaction_active = 0;
        server->transaction_backup_count = 0U;
    }
    server_unlock(server);
    return status;
}
#endif

UmiStatus umi_data_server_commit(UmiDataServer *server)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    if (!server->transaction_active) { server_unlock(server); return UMI_STATUS_INVALID_STATE; }
#ifdef UMICOM_HAS_SQLITE
    if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        int code = sqlite3_exec(server->sqlite, "COMMIT;", NULL, NULL, NULL);
        if (code != SQLITE_OK) status = DataSqlError(server, code);
    }
#endif
    if (status == UMI_STATUS_OK) DataTransactionClear(server);
    else DataObserveAbort(server);
    server_unlock(server);
    return status;
}

/*
 * Provide the data server rollback operation used by this module and its client
 * applications.
 */
/* Superseded implementation retained for engineering review.
 * The former implementation cleared its state even after a failed SQLite rollback. The replacement retains ownership and blocks ordinary work while the backend transaction remains active.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_rollback(UmiDataServer *server)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    /* Apply this operation only while the related capability or state is available. */
    if (!server->transaction_active) {
        server_unlock(server);
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        (void)memcpy(server->records,
                     server->transaction_backup,
                     UMI_DATA_MAX_RECORDS * sizeof(server->records[0]));
        server->count = server->transaction_backup_count;
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE &&
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
#endif

UmiStatus umi_data_server_rollback(UmiDataServer *server)
{
    if (server == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DataEnter(server, 1);
    if (status != UMI_STATUS_OK) return status;
    if (server->transaction_lost) {
        DataMessage(server, "SQLite already ended this transaction; rollback was not performed");
        DataTransactionClear(server);
        server_unlock(server);
        return UMI_STATUS_IO_ERROR;
    }
    if (!server->transaction_active) { server_unlock(server); return UMI_STATUS_INVALID_STATE; }
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        memcpy(server->records, server->transaction_backup,
            UMI_DATA_MAX_RECORDS * sizeof server->records[0]);
        server->count = server->transaction_backup_count;
        DataTransactionClear(server);
    }
#ifdef UMICOM_HAS_SQLITE
    else {
        int code = sqlite3_exec(server->sqlite, "ROLLBACK;", NULL, NULL, NULL);
        if (code != SQLITE_OK) status = DataSqlError(server, code);
        if (sqlite3_get_autocommit(server->sqlite)) DataTransactionClear(server);
        else {
            server->rollback_failed = 1;
            if (status == UMI_STATUS_OK) {
                DataMessage(server, "Rollback returned without ending the transaction");
                status = UMI_STATUS_IO_ERROR;
            }
        }
    }
#endif
    server_unlock(server);
    return status;
}

/*
 * Provide the data server in transaction operation used by this module and its client
 * applications.
 */
/* Superseded implementation retained for engineering review.
 * The former unlocked load raced with begin/commit/rollback. Observations now use the same lock.
 * The active implementation below keeps the public entry point. */
#if 0
int umi_data_server_in_transaction(const UmiDataServer *server)
{
    return server != NULL ? server->transaction_active : 0;
}
#endif

int umi_data_server_in_transaction(const UmiDataServer *source)
{
    if (source == NULL) return 0;
    UmiDataServer *server = (UmiDataServer *)source;
    if (DataInVisitor(server)) return server->transaction_active;
    server_lock(server);
    int active = server->transaction_active;
    server_unlock(server);
    return active;
}

/*
 * Perform data server through the module contract so client applications do not duplicate
 * its policy.
 */
/* Superseded implementation retained for engineering review.
 * The trusted SQL entry point must obey transaction ownership and reflect explicit SQL transaction changes. It is not an application SQL sandbox.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_execute(UmiDataServer *server, const char *sql)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || sql == NULL || sql[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#ifndef UMICOM_HAS_SQLITE
    (void)server;
    (void)sql;
    return UMI_STATUS_UNAVAILABLE;
#else
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend != UMI_DATA_BACKEND_SQLITE) {
        return UMI_STATUS_UNAVAILABLE;
    }
    server_lock(server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (sqlite3_exec(server->sqlite, sql, NULL, NULL, NULL) != SQLITE_OK) {
        set_error(server, sqlite3_errmsg(server->sqlite));
        server_unlock(server);
        return UMI_STATUS_IO_ERROR;
    }
    server_unlock(server);
    return UMI_STATUS_OK;
#endif
}
#endif

UmiStatus umi_data_server_execute(UmiDataServer *server, const char *sql)
{
    if (server == NULL || sql == NULL || sql[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
#ifndef UMICOM_HAS_SQLITE
    status = UMI_STATUS_UNAVAILABLE;
#else
    if (server->backend != UMI_DATA_BACKEND_SQLITE) status = UMI_STATUS_UNAVAILABLE;
    else {
        int before = server->transaction_active;
        int code = sqlite3_exec(server->sqlite, sql, NULL, NULL, NULL);
        int active = !sqlite3_get_autocommit(server->sqlite);
        if (code != SQLITE_OK) status = DataSqlError(server, code);
        if (active) {
            server->transaction_active = 1;
            server->transaction_owner = DataThreadToken();
        } else if (before && code != SQLITE_OK) {
            server->transaction_active = 0;
            server->transaction_lost = 1;
        } else DataTransactionClear(server);
    }
#endif
    server_unlock(server);
    return status;
}

/* Provide the data server path operation used by this module and its client applications. */
const char *umi_data_server_path(const UmiDataServer *server)
{
    return server != NULL ? server->path : "";
}

/*
 * Provide the data server last error operation used by this module and its client
 * applications.
 */
/* Superseded implementation retained for engineering review.
 * Returning mutable shared diagnostic storage raced with other operations. The legacy entry now returns a per-thread copy, valid until its next call on that thread.
 * The active implementation below keeps the public entry point. */
#if 0
const char *umi_data_server_last_error(const UmiDataServer *server)
{
    return server != NULL ? server->last_error : "";
}
#endif

const char *umi_data_server_last_error(const UmiDataServer *server)
{
    static _Thread_local char copied[UMI_DATA_ERROR_CAPACITY];
    copied[0] = '\0';
    if (server != NULL) (void)UmiDataServerCopyError(server, copied, sizeof copied);
    return copied;
}

/* Provide the data server visit operation used by this module and its client applications. */
/* Superseded implementation retained for engineering review.
 * Enumeration must not expose another worker pending data; recursive callbacks must not deadlock the server lock.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_visit(const UmiDataServer *server_const,
                                UmiDataServerRecordVisitor visitor,
                                void *user_data)
{
    UmiDataServer *server = (UmiDataServer *)server_const;
    UmiStatus status = UMI_STATUS_OK;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || visitor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    server_lock(server);
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!server->records[index].used) continue;
            status = visitor(server->records[index].key,
                             server->records[index].value,
                             user_data);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        /* Apply this branch only when its contract condition is satisfied. */
        if (sqlite3_prepare(server->sqlite,
                               "SELECT key,value FROM umicom_kv ORDER BY key;",
                               -1, &statement, NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* The former loop could report a complete successful enumeration
             * after sqlite3_step failed. The explicit result loop replaces it;
             * the previous implementation remains disabled for review, not
             * physically deleted as part of this correctness repair. */
#if 0
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (sqlite3_step(statement) == SQLITE_ROW) {
                const unsigned char *key = sqlite3_column_text(statement, 0);
                const unsigned char *value = sqlite3_column_text(statement, 1);
                status = visitor((const char *)(key != NULL ? key : (const unsigned char *)""),
                                 (const char *)(value != NULL ? value : (const unsigned char *)""),
                                 user_data);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) break;
            }
#endif
            /* Repositories require a complete enumeration or an explicit error.
             * A visitor failure still stops immediately with its own status. */
            for (;;) {
                int step_result = sqlite3_step(statement);
                const unsigned char *key;
                const unsigned char *value;
                int key_bytes, value_bytes;
                if (step_result == SQLITE_DONE) break;
                if (step_result != SQLITE_ROW) {
                    set_error(server, sqlite3_errmsg(server->sqlite));
                    status = UMI_STATUS_IO_ERROR;
                    break;
                }
                key = sqlite3_column_text(statement, 0);
                value = sqlite3_column_text(statement, 1);
                key_bytes = sqlite3_column_bytes(statement, 0);
                value_bytes = sqlite3_column_bytes(statement, 1);
                if (key == NULL || value == NULL || key_bytes < 0 || value_bytes < 0) {
                    set_error(server, "SQLite text conversion failed during enumeration");
                    status = UMI_STATUS_IO_ERROR;
                    break;
                }
                if (memchr(key, 0, (size_t)key_bytes) != NULL ||
                    memchr(value, 0, (size_t)value_bytes) != NULL) {
                    set_error(server, "Stored key/value contains an embedded NUL");
                    status = UMI_STATUS_PARSE_ERROR;
                    break;
                }
                status = visitor((const char *)key, (const char *)value, user_data);
                if (status != UMI_STATUS_OK) break;
            }
            (void)sqlite3_finalize(statement);
        }
    }
#endif
    server_unlock(server);
    return status;
}
#endif

UmiStatus umi_data_server_visit(const UmiDataServer *server_const,
                                UmiDataServerRecordVisitor visitor,
                                void *user_data)
{
    UmiDataServer *server = (UmiDataServer *)server_const;
    UmiStatus status = UMI_STATUS_OK;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || visitor == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    DataVisitFrame frame = { server, DataVisits };
    DataVisits = &frame;
    /* Apply this branch only when its contract condition is satisfied. */
    if (server->backend == UMI_DATA_BACKEND_MEMORY) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < UMI_DATA_MAX_RECORDS; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!server->records[index].used) continue;
            status = visitor(server->records[index].key,
                             server->records[index].value,
                             user_data);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) break;
        }
    }
#ifdef UMICOM_HAS_SQLITE
    else /* Apply this branch only when its contract condition is satisfied. */ if (server->backend == UMI_DATA_BACKEND_SQLITE) {
        sqlite3_stmt *statement = NULL;
        /* Apply this branch only when its contract condition is satisfied. */
        if (sqlite3_prepare(server->sqlite,
                               "SELECT key,value FROM umicom_kv ORDER BY key;",
                               -1, &statement, NULL) != SQLITE_OK) {
            set_error(server, sqlite3_errmsg(server->sqlite));
            status = UMI_STATUS_IO_ERROR;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            /* The former loop could report a complete successful enumeration
             * after sqlite3_step failed. The explicit result loop replaces it;
             * the previous implementation remains disabled for review, not
             * physically deleted as part of this correctness repair. */
#if 0
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
            while (sqlite3_step(statement) == SQLITE_ROW) {
                const unsigned char *key = sqlite3_column_text(statement, 0);
                const unsigned char *value = sqlite3_column_text(statement, 1);
                status = visitor((const char *)(key != NULL ? key : (const unsigned char *)""),
                                 (const char *)(value != NULL ? value : (const unsigned char *)""),
                                 user_data);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK) break;
            }
#endif
            /* Repositories require a complete enumeration or an explicit error.
             * A visitor failure still stops immediately with its own status. */
            for (;;) {
                int step_result = sqlite3_step(statement);
                const unsigned char *key;
                const unsigned char *value;
                int key_bytes, value_bytes;
                if (step_result == SQLITE_DONE) break;
                if (step_result != SQLITE_ROW) {
                    set_error(server, sqlite3_errmsg(server->sqlite));
                    status = UMI_STATUS_IO_ERROR;
                    break;
                }
                key = sqlite3_column_text(statement, 0);
                value = sqlite3_column_text(statement, 1);
                key_bytes = sqlite3_column_bytes(statement, 0);
                value_bytes = sqlite3_column_bytes(statement, 1);
                if (key == NULL || value == NULL || key_bytes < 0 || value_bytes < 0) {
                    set_error(server, "SQLite text conversion failed during enumeration");
                    status = UMI_STATUS_IO_ERROR;
                    break;
                }
                if (memchr(key, 0, (size_t)key_bytes) != NULL ||
                    memchr(value, 0, (size_t)value_bytes) != NULL) {
                    set_error(server, "Stored key/value contains an embedded NUL");
                    status = UMI_STATUS_PARSE_ERROR;
                    break;
                }
                status = visitor((const char *)key, (const char *)value, user_data);
                if (status != UMI_STATUS_OK) break;
            }
            (void)sqlite3_finalize(statement);
        }
    }
#endif
    DataVisits = frame.previous;
    DataObserveAbort(server);
    server_unlock(server);
    return status;
}

/*
 * Provide the data server snapshot operation used by this module and its client
 * applications.
 */
/* Superseded implementation retained for engineering review.
 * Several independent observations could form an inconsistent snapshot and suppress count errors. The replacement captures the complete result under one lock.
 * The active implementation below keeps the public entry point. */
#if 0
UmiStatus umi_data_server_snapshot(const UmiDataServer *server,
                                   UmiDataServerSnapshot *out_snapshot)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(out_snapshot->backend_name)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(out_snapshot->path, sizeof(out_snapshot->path), "%s",
                       umi_data_server_path(server));
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(out_snapshot->path)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    written = snprintf(out_snapshot->last_error,
                       sizeof(out_snapshot->last_error), "%s",
                       umi_data_server_last_error(server));
    return written < 0 || (size_t)written >= sizeof(out_snapshot->last_error)
        ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
#endif

UmiStatus umi_data_server_snapshot(const UmiDataServer *source, UmiDataServerSnapshot *out)
{
    if (source == NULL || out == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out, 0, sizeof *out);
    UmiDataServer *server = (UmiDataServer *)source;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    status = DataCountLocked(server, &out->record_count);
    if (status == UMI_STATUS_OK) {
        out->backend = server->backend;
        out->transaction_active = server->transaction_active;
        (void)snprintf(out->backend_name, sizeof out->backend_name, "%s", umi_data_server_backend_name(server));
        memcpy(out->path, server->path, sizeof out->path);
        memcpy(out->last_error, server->last_error, sizeof out->last_error);
    }
    DataObserveAbort(server);
    server_unlock(server);
    return status;
}


/* Additive checked observations. Legacy scalar getters remain available. */
UmiStatus UmiDataServerCountChecked(const UmiDataServer *source, size_t *outCount)
{
    if (outCount != NULL) *outCount = 0;
    if (source == NULL || outCount == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDataServer *server = (UmiDataServer *)source;
    UmiStatus status = DataEnter(server, 0);
    if (status != UMI_STATUS_OK) return status;
    status = DataCountLocked(server, outCount);
    if (status != UMI_STATUS_OK) *outCount = 0;
    DataObserveAbort(server);
    server_unlock(server);
    return status;
}
UmiStatus UmiDataServerCopyError(const UmiDataServer *source, char *outText, size_t capacity)
{
    if (outText != NULL && capacity != 0) outText[0] = '\0';
    if (source == NULL || outText == NULL || capacity == 0) return UMI_STATUS_INVALID_ARGUMENT;
    UmiDataServer *server = (UmiDataServer *)source;
    int borrowedLock = DataInVisitor(server);
    if (!borrowedLock) server_lock(server);
    size_t length = strlen(server->last_error);
    UmiStatus status = length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK) memcpy(outText, server->last_error, length + 1U);
    if (!borrowedLock) server_unlock(server);
    return status;
}

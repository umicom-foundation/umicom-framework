/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/local_profile_database.c
 * PURPOSE: Store local password verifiers in SQLite while preserving native profile compatibility.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "local_profile_platform_internal.h"
#include "umicom/platform/path.h"
#include "umicom/security/local_profile_database.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>
#ifdef UMI_LOCAL_PROFILE_HAS_SQLITE
#include <sqlite3.h>
typedef struct ProfileDatabase
{
    sqlite3 *database;
    UmiLocalProfileBackend native;
    char application[96];
} ProfileDatabase;

/* Translate storage failures without exposing usernames, SQL text or verifier
 * bytes in user-facing error messages. A locked store can be retried later. */
static UmiStatus DatabaseStatus(int result)
{
    int primary = result & 255;
    if (primary == SQLITE_OK || primary == SQLITE_DONE)
        return UMI_STATUS_OK;
    if (primary == SQLITE_BUSY || primary == SQLITE_LOCKED)
        return UMI_STATUS_BUSY;
    if (primary == SQLITE_NOMEM)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (primary == SQLITE_CORRUPT || primary == SQLITE_NOTADB)
        return UMI_STATUS_PARSE_ERROR;
    return UMI_STATUS_IO_ERROR;
}
static int DatabaseBindIdentity(ProfileDatabase *store, sqlite3_stmt *statement, const char *name)
{
    int result = sqlite3_bind_text(statement, 1, store->application, -1, SQLITE_TRANSIENT);
    if (result == SQLITE_OK)
        result = sqlite3_bind_text(statement, 2, name, -1, SQLITE_TRANSIENT);
    return result;
}
static int DatabaseBindRecord(sqlite3_stmt *statement, const UmiLocalProfileRecord *record)
{
    int result = sqlite3_bind_int64(statement, 3, record->version);
    if (result == SQLITE_OK)
        result = sqlite3_bind_int64(statement, 4, record->iterations);
    if (result == SQLITE_OK)
        result = sqlite3_bind_blob(statement, 5, record->salt, 32, SQLITE_TRANSIENT);
    if (result == SQLITE_OK)
        result = sqlite3_bind_blob(statement, 6, record->verifier, 32, SQLITE_TRANSIENT);
    return result;
}
/* Native-width structures are never serialized. Validate every stored field
 * before using it as a cryptographic work factor or copying a blob. */
static UmiStatus DatabaseReadRecord(ProfileDatabase *store, const char *name,
                                    UmiLocalProfileRecord *out)
{
    sqlite3_stmt *statement = NULL;
    int result = sqlite3_prepare_v2(store->database,
                                    "SELECT format, iterations, salt, verifier FROM local_profiles "
                                    "WHERE application_id=?1 AND username=?2",
                                    -1, &statement, NULL);
    if (result == SQLITE_OK)
        result = DatabaseBindIdentity(store, statement, name);
    if (result == SQLITE_OK)
        result = sqlite3_step(statement);
    UmiStatus status = result == SQLITE_DONE ? UMI_STATUS_NOT_FOUND : DatabaseStatus(result);
    if (result == SQLITE_ROW)
    {
        UmiLocalProfileRecord record = {0};
        if (sqlite3_column_type(statement, 0) != SQLITE_INTEGER ||
            sqlite3_column_int64(statement, 0) != 1 ||
            sqlite3_column_type(statement, 1) != SQLITE_INTEGER ||
            sqlite3_column_int64(statement, 1) != UMI_LOCAL_PROFILE_ITERATIONS ||
            sqlite3_column_type(statement, 2) != SQLITE_BLOB ||
            sqlite3_column_bytes(statement, 2) != 32 ||
            sqlite3_column_type(statement, 3) != SQLITE_BLOB ||
            sqlite3_column_bytes(statement, 3) != 32 || sqlite3_column_blob(statement, 2) == NULL ||
            sqlite3_column_blob(statement, 3) == NULL)
        {
            status = UMI_STATUS_PARSE_ERROR;
        }
        else
        {
            record.version = 1U;
            record.iterations = UMI_LOCAL_PROFILE_ITERATIONS;
            memcpy(record.salt, sqlite3_column_blob(statement, 2), sizeof(record.salt));
            memcpy(record.verifier, sqlite3_column_blob(statement, 3), sizeof(record.verifier));
            *out = record;
            status = UMI_STATUS_OK;
        }
        umi_secret_clear(&record, sizeof(record));
    }
    sqlite3_finalize(statement);
    return status;
}
static UmiStatus DatabaseRead(void *context, const char *name, UmiLocalProfileRecord *out)
{
    ProfileDatabase *store = context;
    UmiStatus status = DatabaseReadRecord(store, name, out);
    /* Legacy profiles remain in their original vault. A corrupt database record
     * must not silently fall back to a different credential for the same name. */
    return status == UMI_STATUS_NOT_FOUND ? store->native.read(store->native.context, name, out)
                                          : status;
}
static UmiStatus DatabaseCreate(void *context, const char *name,
                                const UmiLocalProfileRecord *record)
{
    ProfileDatabase *store = context;
    UmiStatus status = UmiLocalProfilePlatformEnter(store->native.context);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLocalProfileRecord previous = {0};
    status = store->native.read(store->native.context, name, &previous);
    umi_secret_clear(&previous, sizeof(previous));
    if (status == UMI_STATUS_OK)
        status = UMI_STATUS_ALREADY_EXISTS;
    else if (status == UMI_STATUS_NOT_FOUND)
    {
        sqlite3_stmt *statement = NULL;
        int result = sqlite3_prepare_v2(
            store->database,
            "INSERT INTO local_profiles(application_id,username,format,iterations,salt,verifier)"
            " VALUES(?1,?2,?3,?4,?5,?6)",
            -1, &statement, NULL);
        if (result == SQLITE_OK)
            result = DatabaseBindIdentity(store, statement, name);
        if (result == SQLITE_OK)
            result = DatabaseBindRecord(statement, record);
        if (result == SQLITE_OK)
            result = sqlite3_step(statement);
        status = (result == SQLITE_CONSTRAINT_PRIMARYKEY || result == SQLITE_CONSTRAINT_UNIQUE)
                     ? UMI_STATUS_ALREADY_EXISTS
                     : DatabaseStatus(result);
        sqlite3_finalize(statement);
    }
    UmiLocalProfilePlatformLeave(store->native.context);
    return status;
}
static UmiStatus DatabaseRemove(void *context, const char *name,
                                const UmiLocalProfileRecord *expected)
{
    ProfileDatabase *store = context;
    UmiStatus status = UmiLocalProfilePlatformEnter(store->native.context);
    if (status != UMI_STATUS_OK)
        return status;
    UmiLocalProfileRecord current = {0};
    status = DatabaseReadRecord(store, name, &current);
    umi_secret_clear(&current, sizeof(current));
    if (status == UMI_STATUS_NOT_FOUND)
        status = store->native.remove(store->native.context, name, expected);
    else if (status == UMI_STATUS_OK)
    {
        /* Compare in the same DELETE statement. A record replaced after password
         * verification must not be removed by an older authentication result. */
        sqlite3_stmt *statement = NULL;
        int result =
            sqlite3_prepare_v2(store->database,
                               "DELETE FROM local_profiles WHERE application_id=?1 AND username=?2 "
                               "AND format=?3 AND iterations=?4 AND salt=?5 AND verifier=?6",
                               -1, &statement, NULL);
        if (result == SQLITE_OK)
            result = DatabaseBindIdentity(store, statement, name);
        if (result == SQLITE_OK)
            result = DatabaseBindRecord(statement, expected);
        if (result == SQLITE_OK)
            result = sqlite3_step(statement);
        status = DatabaseStatus(result);
        if (status == UMI_STATUS_OK && sqlite3_changes(store->database) != 1)
            status = UMI_STATUS_INVALID_STATE;
        sqlite3_finalize(statement);
    }
    UmiLocalProfilePlatformLeave(store->native.context);
    return status;
}
static UmiStatus DatabaseRandom(void *context, unsigned char *out, size_t size)
{
    ProfileDatabase *store = context;
    return store->native.random(store->native.context, out, size);
}
static UmiStatus DatabaseDerive(void *context, const char *password, size_t length,
                                const UmiLocalProfileRecord *record, unsigned char out[32])
{
    ProfileDatabase *store = context;
    return store->native.derive(store->native.context, password, length, record, out);
}
static void DatabaseDestroy(void *context)
{
    ProfileDatabase *store = context;
    if (store == NULL)
        return;
    if (store->database != NULL)
        sqlite3_close_v2(store->database);
    if (store->native.destroy != NULL)
        store->native.destroy(store->native.context);
    umi_secret_clear(store, sizeof(*store));
    free(store);
}
#endif
UmiStatus UmiLocalProfileStoreDatabase(const char *path, const char *application_id,
                                       UmiLocalProfileStore **out_store)
{
    if (out_store == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_store = NULL;
    if (path == NULL || !umi_path_is_absolute(path) || application_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
#ifdef UMI_LOCAL_PROFILE_HAS_SQLITE
    ProfileDatabase *store = calloc(1U, sizeof(*store));
    if (store == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiLocalProfilePlatformBackend(application_id, &store->native);
    if (status != UMI_STATUS_OK)
    {
        DatabaseDestroy(store);
        return status;
    }
    /* The native factory has already bounded and validated this namespace. */
    memcpy(store->application, application_id, strlen(application_id) + 1U);
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX;
#ifdef SQLITE_OPEN_NOFOLLOW
    flags |= SQLITE_OPEN_NOFOLLOW;
#endif
    int result = sqlite3_open_v2(path, &store->database, flags, NULL);
    if (result == SQLITE_OK)
        result = sqlite3_extended_result_codes(store->database, 1);
    if (result == SQLITE_OK)
        result = sqlite3_busy_timeout(store->database, 0);
    if (result == SQLITE_OK)
        result = sqlite3_exec(store->database,
                              "PRAGMA trusted_schema=OFF; PRAGMA secure_delete=ON;"
                              "CREATE TABLE IF NOT EXISTS local_profiles("
                              "application_id TEXT NOT NULL, username TEXT NOT NULL, "
                              "format INTEGER NOT NULL, iterations INTEGER NOT NULL, "
                              "salt BLOB NOT NULL CHECK(length(salt)=32), verifier BLOB NOT NULL "
                              "CHECK(length(verifier)=32),"
                              "PRIMARY KEY(application_id,username)) WITHOUT ROWID;",
                              NULL, NULL, NULL);
    /* Startup must not wait behind another process's schema lock. Once open,
     * worker-thread account operations may wait for a short storage transaction. */
    if (result == SQLITE_OK)
        result = sqlite3_busy_timeout(store->database, 5000);
    status = DatabaseStatus(result);
    if (status == UMI_STATUS_OK)
    {
        UmiLocalProfileBackend backend = {store,          DatabaseRead,   DatabaseCreate,
                                          DatabaseRemove, DatabaseRandom, DatabaseDerive,
                                          DatabaseDestroy};
        status = UmiLocalProfileStoreCreate(&backend, out_store);
    }
    if (status != UMI_STATUS_OK)
        DatabaseDestroy(store);
    return status;
#else
    (void)application_id;
    return UMI_STATUS_UNAVAILABLE;
#endif
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_profile/test_database.c
 * PURPOSE: Exercise local account database persistence, isolation and corrupted-record rejection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/native_arguments.h"
#include "umicom/security/local_profile_database.h"
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
/* Synthetic fixture credentials are never broker credentials. No network or
 * broker adapter is opened by this test. Each CTest run owns a fresh database. */
static const char password[] = "fixture-only local password";
static int Run(int argc, char **argv)
{
    if (argc != 4)
        return 2;
    int failed = 0;
    UmiLocalProfileStore *first = NULL, *second = NULL;
    sqlite3 *database = NULL;
    UmiStatus status = UmiLocalProfileStoreDatabase(argv[1], argv[3], &first);
    if (status == UMI_STATUS_UNAVAILABLE)
        return 77;
    CHECK(status == UMI_STATUS_OK);
    CHECK(UmiLocalProfileRegister(first, "Alice", password) == UMI_STATUS_OK);
    CHECK(UmiLocalProfileVerify(first, "ALICE", password) == UMI_STATUS_OK);
    CHECK(UmiLocalProfileVerify(first, "alice", "another fixture password") ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalProfileRegister(first, "alice", password) == UMI_STATUS_ALREADY_EXISTS);
    if (strcmp(argv[2], "reopen") == 0)
    {
        UmiLocalProfileStoreRelease(first);
        first = NULL;
        CHECK(UmiLocalProfileStoreDatabase(argv[1], argv[3], &first) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileVerify(first, "alice", password) == UMI_STATUS_OK);
    }
    else if (strcmp(argv[2], "duplicate") == 0)
    {
        CHECK(UmiLocalProfileStoreDatabase(argv[1], argv[3], &second) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileRegister(second, "alice", "another fixture password") ==
              UMI_STATUS_ALREADY_EXISTS);
        CHECK(UmiLocalProfileVerify(second, "alice", password) == UMI_STATUS_OK);
    }
    else if (strcmp(argv[2], "namespace") == 0)
    {
        char other[96];
        CHECK(snprintf(other, sizeof(other), "%s.other", argv[3]) > 0);
        CHECK(UmiLocalProfileStoreDatabase(argv[1], other, &second) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileVerify(second, "alice", password) == UMI_STATUS_PERMISSION_DENIED);
        CHECK(UmiLocalProfileRegister(second, "alice", "another fixture password") ==
              UMI_STATUS_OK);
        CHECK(UmiLocalProfileVerify(first, "alice", password) == UMI_STATUS_OK);
        CHECK(UmiLocalProfileRemove(second, "alice", "another fixture password") == UMI_STATUS_OK);
    }
    else if (strcmp(argv[2], "corrupt") == 0)
    {
        CHECK(sqlite3_open_v2(argv[1], &database, SQLITE_OPEN_READWRITE, NULL) == SQLITE_OK);
        CHECK(sqlite3_exec(database, "UPDATE local_profiles SET iterations=1", NULL, NULL, NULL) ==
              SQLITE_OK);
        CHECK(UmiLocalProfileVerify(first, "alice", password) == UMI_STATUS_PARSE_ERROR);
        CHECK(UmiLocalProfileRemove(first, "alice", password) == UMI_STATUS_PARSE_ERROR);
        goto cleanup;
    }
    else if (strcmp(argv[2], "verifier") == 0)
    {
        CHECK(UmiLocalProfileRegister(first, "bob", password) == UMI_STATUS_OK);
        sqlite3_stmt *statement = NULL;
        CHECK(sqlite3_open_v2(argv[1], &database, SQLITE_OPEN_READONLY, NULL) == SQLITE_OK);
        CHECK(sqlite3_prepare_v2(
                  database,
                  "SELECT count(*), count(DISTINCT hex(salt)), count(DISTINCT hex(verifier)), "
                  "min(length(salt)), min(length(verifier)) FROM local_profiles",
                  -1, &statement, NULL) == SQLITE_OK);
        int row = sqlite3_step(statement);
        /* Equal passwords must produce distinct salts and derived values.
         * This catches accidental reuse of a fixed salt or a plaintext value. */
        int valid =
            row == SQLITE_ROW && sqlite3_column_int(statement, 0) == 2 &&
            sqlite3_column_int(statement, 1) == 2 && sqlite3_column_int(statement, 2) == 2 &&
            sqlite3_column_int(statement, 3) == 32 && sqlite3_column_int(statement, 4) == 32;
        sqlite3_finalize(statement);
        CHECK(valid);
    }
    else if (strcmp(argv[2], "remove") != 0)
        return 2;
    CHECK(UmiLocalProfileRemove(first, "alice", "another fixture password") ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalProfileVerify(first, "alice", password) == UMI_STATUS_OK);
    CHECK(UmiLocalProfileRemove(first, "alice", password) == UMI_STATUS_OK);
    CHECK(UmiLocalProfileVerify(first, "alice", password) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(UmiLocalProfileRegister(first, "alice", "replacement fixture password") == UMI_STATUS_OK);
    CHECK(UmiLocalProfileRemove(first, "alice", "replacement fixture password") == UMI_STATUS_OK);
cleanup:
    if (database != NULL)
        sqlite3_close_v2(database);
    UmiLocalProfileStoreRelease(second);
    UmiLocalProfileStoreRelease(first);
    return failed;
}
int main(int argc, char **argv)
{
    int result = 1;
    UmiStatus status = UmiNativeArgumentsDispatch(argc, argv, Run, &result);
    return status == UMI_STATUS_OK ? result : 1;
}

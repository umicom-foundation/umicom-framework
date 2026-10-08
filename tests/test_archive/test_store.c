/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_store.c
 * PURPOSE: Exercise privacy, corrupt storage, atomic cancellation and precise result identity.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    UmiDataServer *server = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveOrigin origin = FixtureOrigin(true);
    UmiTestArchiveEntry entry = {0}, unchanged = {0};
    unchanged.id = 999;
    UmiTestResult *result = calloc(1, sizeof(*result));
    CHECK(result != NULL);
    UmiTestArchiveCatalog *catalog = calloc(1, sizeof(*catalog));
    CHECK(catalog != NULL);
    uint32_t attempt = 99;
    if (strcmp(name, "invalid") == 0)
    {
        UmiTestArchive *other = archive;
        CHECK(UmiTestArchiveCreate(server, "../scope", &other) == UMI_STATUS_INVALID_ARGUMENT &&
              other == NULL);
        CHECK(UmiTestArchiveSave(archive, NULL, &origin, NULL, &unchanged) == UMI_STATUS_INVALID_ARGUMENT &&
              unchanged.id == 999);
        CHECK(UmiTestArchiveRead(archive, 0, &unchanged) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(name, "active") == 0)
    {
        job->snapshot.state = UMI_TASK_RUNNING;
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &unchanged) == UMI_STATUS_BUSY &&
              unchanged.id == 999);
        FixtureEmpty(archive);
    }
    else if (strcmp(name, "outer-transaction") == 0)
    {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &unchanged) == UMI_STATUS_BUSY);
        CHECK(umi_data_server_in_transaction(server));
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    }
    else if (strcmp(name, "cancel-before") == 0 || strcmp(name, "cancel-during") == 0)
    {
        UmiCancellationToken *token = NULL;
        CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
        if (strcmp(name, "cancel-before") == 0)
            umi_cancellation_token_request(token);
        else
        {
            job->cancel = token;
            job->cancel_after = 1;
        }
        CHECK(UmiTestArchiveSave(archive, job, &origin, token, &unchanged) == UMI_STATUS_CANCELLED &&
              unchanged.id == 999);
        CHECK(umi_data_server_count(server) == 0);
        FixtureEmpty(archive);
        umi_cancellation_token_destroy(token);
    }
    else if (strcmp(name, "identity") == 0 || strcmp(name, "summary") == 0)
    {
        if (strcmp(name, "identity") == 0)
            strcpy(job->results[2].test_id, "other");
        else
            ++job->snapshot.duration_ms;
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &unchanged) == UMI_STATUS_INVALID_STATE &&
              unchanged.id == 999);
        CHECK(umi_data_server_count(server) == 0);
    }
    else if (strcmp(name, "capacity") == 0)
    {
        for (size_t i = 0; i < UMI_TEST_ARCHIVE_CAPACITY; ++i)
            CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &unchanged) == UMI_STATUS_CAPACITY_EXCEEDED &&
              unchanged.id == 999);
        CHECK(UmiTestArchiveList(archive, catalog) == UMI_STATUS_OK &&
              catalog->count == UMI_TEST_ARCHIVE_CAPACITY);
    }
    else if (strcmp(name, "memory-full") == 0)
    {
        /* Unrelated values share the bounded memory store. A failed archive
         * must not evict them or leave its first request behind. */
        for (size_t i = 0; i < 2047U; ++i)
        {
            char key[64];
            (void)snprintf(key, sizeof(key), "unrelated/%zu", i);
            CHECK(umi_data_server_set(server, key, "retained") == UMI_STATUS_OK);
        }
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &unchanged) == UMI_STATUS_CAPACITY_EXCEEDED &&
              unchanged.id == 999);
        CHECK(umi_data_server_count(server) == 2047U);
        FixtureEmpty(archive);
    }
    else if (strcmp(name, "scope-boundary") == 0)
    {
        char scope[66];
        memset(scope, 'a', 64U);
        scope[64] = '\0';
        UmiTestArchive *other = NULL;
        CHECK(UmiTestArchiveCreate(server, scope, &other) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveSave(other, job, &origin, NULL, &entry) == UMI_STATUS_OK);
        UmiTestArchiveDestroy(other);
        scope[64] = 'a';
        scope[65] = '\0';
        CHECK(UmiTestArchiveCreate(server, scope, &other) == UMI_STATUS_INVALID_ARGUMENT && other == NULL);
    }
    else if (strcmp(name, "exhausted-id") == 0)
    {
        CHECK(umi_data_server_set(server, "ctest-archive/test/next", "1|18446744073709551615") ==
              UMI_STATUS_OK);
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &unchanged) == UMI_STATUS_CAPACITY_EXCEEDED &&
              unchanged.id == 999);
    }
    else
    {
        if (strcmp(name, "privacy") == 0)
            origin.retain_output = false;
        if (strcmp(name, "maximal-fields") == 0)
        {
            /* Metadata and requests must fit Data Server's existing value
             * limit even when every documented text field is full. */
            memset(origin.source_root, 's', sizeof(origin.source_root) - 1U);
            memset(origin.source_revision, 'r', sizeof(origin.source_revision) - 1U);
            origin.source_root[sizeof(origin.source_root) - 1U] = '\0';
            origin.source_revision[sizeof(origin.source_revision) - 1U] = '\0';
            for (size_t i = 0; i < 2U; ++i)
            {
                UmiCtestJobRequest *request = &job->requests[i];
                memset(request->test_id, i == 0 ? 'a' : 'b', sizeof(request->test_id) - 1U);
                memset(request->name, 'n', sizeof(request->name) - 1U);
                memset(request->build_directory, 'd', sizeof(request->build_directory) - 1U);
                memset(request->configuration, 'c', sizeof(request->configuration) - 1U);
                request->test_id[sizeof(request->test_id) - 1U] = '\0';
                request->name[sizeof(request->name) - 1U] = '\0';
                request->build_directory[sizeof(request->build_directory) - 1U] = '\0';
                request->configuration[sizeof(request->configuration) - 1U] = '\0';
            }
            for (size_t i = 0; i < 4U; ++i)
            {
                strcpy(job->results[i].test_id, job->requests[i % 2U].test_id);
                strcpy(job->results[i].name, job->requests[i % 2U].name);
            }
        }
        if (strcmp(name, "large-output") == 0)
        {
            memset(job->results[0].output, 'x', sizeof(job->results[0].output) - 1U);
            job->results[0].output[sizeof(job->results[0].output) - 1U] = '\0';
        }
        if (strcmp(name, "never-started") == 0)
        {
            job->snapshot.state = UMI_TASK_CANCELLED;
            job->snapshot.status = UMI_STATUS_CANCELLED;
            job->snapshot.first_error = UMI_STATUS_OK;
            job->snapshot.completed = 0;
            job->snapshot.passed = 0;
            job->snapshot.failed = 0;
            job->snapshot.skipped = 0;
            job->snapshot.timed_out = 0;
            job->snapshot.duration_ms = 0;
        }
        CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK && entry.id == 1);
        if (strcmp(name, "roundtrip") == 0 || strcmp(name, "privacy") == 0 ||
            strcmp(name, "large-output") == 0 || strcmp(name, "maximal-fields") == 0)
        {
            CHECK(UmiTestArchiveRead(archive, 1, &unchanged) == UMI_STATUS_OK && unchanged.run.failed == 1 &&
                  unchanged.run.timed_out == 1);
            CHECK(UmiTestArchiveResultAt(archive, 1, 0, result, &attempt) == UMI_STATUS_OK && attempt == 1);
            CHECK(strcmp(result->output, origin.retain_output ? job->results[0].output : "") == 0);
            CHECK(UmiTestArchiveResultAt(archive, 1, 3, result, &attempt) == UMI_STATUS_OK && attempt == 2 &&
                  result->state == UMI_TEST_STATE_TIMED_OUT);
            UmiCtestJobRequest request = {0};
            CHECK(UmiTestArchiveRequestAt(archive, 1, 1, &request) == UMI_STATUS_OK &&
                  strcmp(request.name, job->requests[1].name) == 0);
        }
        else if (strcmp(name, "never-started") == 0)
        {
            CHECK(entry.run.planned == 4 && entry.run.completed == 0);
            CHECK(UmiTestArchiveResultAt(archive, 1, 0, result, &attempt) == UMI_STATUS_NOT_FOUND &&
                  attempt == 99);
            UmiCtestJobRequest request = {0};
            CHECK(UmiTestArchiveRequestAt(archive, 1, 1, &request) == UMI_STATUS_OK);
        }
        else if (strcmp(name, "missing") == 0)
        {
            strcpy(result->name, "unchanged");
            CHECK(UmiTestArchiveResultAt(archive, 1, 4, result, &attempt) == UMI_STATUS_NOT_FOUND);
            CHECK(strcmp(result->name, "unchanged") == 0 && attempt == 99);
            unchanged.id = 999;
            CHECK(UmiTestArchiveRead(archive, 99, &unchanged) == UMI_STATUS_NOT_FOUND && unchanged.id == 999);
        }
        else if (strcmp(name, "namespace") == 0)
        {
            UmiTestArchive *other = NULL;
            CHECK(UmiTestArchiveCreate(server, "other", &other) == UMI_STATUS_OK);
            FixtureEmpty(other);
            CHECK(UmiTestArchiveRemove(other, 1) == UMI_STATUS_NOT_FOUND);
            UmiTestArchiveDestroy(other);
        }
        else if (strcmp(name, "remove") == 0)
        {
            CHECK(UmiTestArchiveRemove(archive, 1) == UMI_STATUS_OK);
            FixtureEmpty(archive);
            CHECK(umi_data_server_count(server) == 1);
            CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK && entry.id == 2);
        }
        else if (strcmp(name, "missing-part") == 0 || strcmp(name, "short-part") == 0 ||
                 strcmp(name, "chunk-overflow") == 0)
        {
            const char *key = "ctest-archive/test/run/1/result/0/part/0";
            if (strcmp(name, "missing-part") == 0)
                CHECK(umi_data_server_delete(server, key) == UMI_STATUS_OK);
            else if (strcmp(name, "short-part") == 0)
                CHECK(umi_data_server_set(server, key, "x") == UMI_STATUS_OK);
            else
                CHECK(umi_data_server_set(server, "ctest-archive/test/run/1/result/0",
                                          "1|18446744073709551615|1|") == UMI_STATUS_OK);
            strcpy(result->name, "unchanged");
            CHECK(UmiTestArchiveResultAt(archive, 1, 0, result, &attempt) == UMI_STATUS_PARSE_ERROR &&
                  strcmp(result->name, "unchanged") == 0);
        }
        else if (strcmp(name, "missing-metadata") == 0 || strcmp(name, "duplicate") == 0 ||
                 strcmp(name, "corrupt") == 0)
        {
            char wire[4096];
            if (strcmp(name, "missing-metadata") == 0)
                CHECK(umi_data_server_delete(server, "ctest-archive/test/next") == UMI_STATUS_OK);
            else if (strcmp(name, "duplicate") == 0)
            {
                CHECK(umi_data_server_get(server, "ctest-archive/test/slot/0", wire, sizeof(wire)) ==
                      UMI_STATUS_OK);
                CHECK(umi_data_server_set(server, "ctest-archive/test/slot/1", wire) == UMI_STATUS_OK);
            }
            else
                CHECK(umi_data_server_set(server, "ctest-archive/test/slot/0", "invalid") == UMI_STATUS_OK);
            catalog->count = 99;
            CHECK(UmiTestArchiveList(archive, catalog) == UMI_STATUS_PARSE_ERROR && catalog->count == 99);
        }
        else
            CHECK(0);
    }
    free(catalog);
    free(result);
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}

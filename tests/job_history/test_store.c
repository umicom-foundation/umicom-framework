/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/job_history/test_store.c
 * PURPOSE: Verify atomic job state transitions, capacity and retention using shared memory storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/data/job_history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(x))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                          \
            exit(1);                                                                                         \
        }                                                                                                    \
    } while (0)
static void Begin(UmiJobHistory *h, UmiJobHistoryEntry *entry)
{
    CHECK(UmiJobHistoryBegin(h, "build.workflow", "Build", 2U, entry) == UMI_STATUS_OK);
}
static void Finish(UmiJobHistory *h, UmiJobHistoryEntry *entry)
{
    CHECK(UmiJobHistoryUpdate(h, entry->id, entry->revision, UMI_JOB_HISTORY_RUNNING, 0, UMI_STATUS_OK,
                              entry) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryUpdate(h, entry->id, entry->revision, UMI_JOB_HISTORY_SUCCEEDED, 2, UMI_STATUS_OK,
                              entry) == UMI_STATUS_OK);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    UmiDataServer *server = NULL;
    UmiJobHistory *history = NULL, *other = NULL;
    UmiJobHistoryEntry entry = {0}, out = {0};
    UmiJobHistorySnapshot *snapshot = calloc(1, sizeof(*snapshot));
    CHECK(snapshot != NULL);
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiJobHistoryCreate(server, "test", &history) == UMI_STATUS_OK);
    if (strcmp(name, "invalid") == 0)
    {
        CHECK(UmiJobHistoryCreate(server, "bad/scope", &other) == UMI_STATUS_INVALID_ARGUMENT &&
              other == NULL);
        CHECK(UmiJobHistoryBegin(history, "bad/key", "caption", 1, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJobHistoryBegin(history, "good", "caption\n", 1, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJobHistoryBegin(history, "good", "caption", 0, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJobHistoryBegin(history, "good", "caption", 65, &out) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 0);
    }
    else if (strcmp(name, "outer-transaction") == 0)
    {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(umi_data_server_set(server, "keep", "value") == UMI_STATUS_OK);
        CHECK(UmiJobHistoryBegin(history, "build", "Build", 1, &out) != UMI_STATUS_OK);
        CHECK(umi_data_server_in_transaction(server));
        char value[16];
        CHECK(umi_data_server_get(server, "keep", value, sizeof(value)) == UMI_STATUS_OK);
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    }
    else if (strcmp(name, "capacity") == 0 || strcmp(name, "prune") == 0 || strcmp(name, "monotonic") == 0)
    {
        for (size_t i = 0; i < UMI_JOB_HISTORY_CAPACITY; ++i)
        {
            Begin(history, &entry);
            if (i != 0 && strcmp(name, "capacity") != 0)
                Finish(history, &entry);
        }
        out = entry;
        CHECK(UmiJobHistoryBegin(history, "build", "Build", 1, &out) == UMI_STATUS_CAPACITY_EXCEEDED &&
              out.id == entry.id);
        size_t removed = 99;
        CHECK(UmiJobHistoryPruneFinished(history, &removed) == UMI_STATUS_OK);
        CHECK(removed == (strcmp(name, "capacity") == 0 ? 0U : UMI_JOB_HISTORY_CAPACITY - 1U));
        CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK);
        CHECK(snapshot->unfinished_count == (strcmp(name, "capacity") == 0 ? UMI_JOB_HISTORY_CAPACITY : 1U));
        if (strcmp(name, "capacity") != 0)
        {
            Begin(history, &entry);
            CHECK(entry.id == UMI_JOB_HISTORY_CAPACITY + 1U);
            CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->entries[0].id == 1U);
        }
    }
    else
    {
        Begin(history, &entry);
        if (strcmp(name, "namespace") == 0)
        {
            CHECK(UmiJobHistoryCreate(server, "other", &other) == UMI_STATUS_OK);
            CHECK(UmiJobHistoryCapture(other, snapshot) == UMI_STATUS_OK && snapshot->count == 0);
            Begin(other, &out);
            CHECK(out.id == 1);
        }
        else if (strcmp(name, "missing-metadata") == 0 || strcmp(name, "corrupt") == 0 ||
                 strcmp(name, "duplicate") == 0 || strcmp(name, "identity-overflow") == 0 ||
                 strcmp(name, "revision-overflow") == 0)
        {
            if (strcmp(name, "missing-metadata") == 0)
                CHECK(umi_data_server_delete(server, "job-history/test/next") == UMI_STATUS_OK);
            else if (strcmp(name, "corrupt") == 0)
                CHECK(umi_data_server_set(server, "job-history/test/slot/0", "bad") == UMI_STATUS_OK);
            else if (strcmp(name, "duplicate") == 0)
            {
                char value[640];
                CHECK(umi_data_server_get(server, "job-history/test/slot/0", value, sizeof(value)) ==
                      UMI_STATUS_OK);
                CHECK(umi_data_server_set(server, "job-history/test/slot/1", value) == UMI_STATUS_OK);
            }
            else if (strcmp(name, "identity-overflow") == 0)
            {
                CHECK(umi_data_server_set(server, "job-history/test/next", "1|18446744073709551615") ==
                      UMI_STATUS_OK);
                CHECK(UmiJobHistoryBegin(history, "build", "Build", 1, &out) == UMI_STATUS_CAPACITY_EXCEEDED);
            }
            else
            {
                CHECK(umi_data_server_set(server, "job-history/test/slot/0",
                                          "1|1|18446744073709551615|1|0|2|0|61|62") == UMI_STATUS_OK);
                CHECK(UmiJobHistoryUpdate(history, 1, UINT64_MAX, UMI_JOB_HISTORY_RUNNING, 0, UMI_STATUS_OK,
                                          &out) == UMI_STATUS_CAPACITY_EXCEEDED);
            }
            if (strcmp(name, "identity-overflow") != 0 && strcmp(name, "revision-overflow") != 0)
            {
                snapshot->count = 17;
                CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_PARSE_ERROR &&
                      snapshot->count == 17);
                CHECK(UmiJobHistoryBegin(history, "build", "Build", 1, &out) == UMI_STATUS_PARSE_ERROR);
                CHECK(!umi_data_server_in_transaction(server));
            }
        }
        else if (strcmp(name, "premature-success") == 0)
        {
            CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED, 2,
                                      UMI_STATUS_OK, &out) == UMI_STATUS_INVALID_STATE);
        }
        else if (strcmp(name, "cancel") == 0 || strcmp(name, "failure") == 0)
        {
            UmiStatus result = strcmp(name, "cancel") == 0 ? UMI_STATUS_CANCELLED : UMI_STATUS_IO_ERROR;
            UmiJobHistoryState state =
                strcmp(name, "cancel") == 0 ? UMI_JOB_HISTORY_CANCELLED : UMI_JOB_HISTORY_FAILED;
            CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, state, 0, result, &out) ==
                  UMI_STATUS_OK);
            CHECK(out.result == result && UmiJobHistoryIsFinished(out.state));
        }
        else
        {
            uint64_t revision = entry.revision;
            CHECK(UmiJobHistoryUpdate(history, entry.id, revision, UMI_JOB_HISTORY_RUNNING, 0, UMI_STATUS_OK,
                                      &entry) == UMI_STATUS_OK);
            if (strcmp(name, "stale") == 0 || strcmp(name, "second-handle") == 0)
            {
                CHECK(UmiJobHistoryCreate(server, "test", &other) == UMI_STATUS_OK);
                out.id = 99;
                CHECK(UmiJobHistoryUpdate(other, entry.id, revision, UMI_JOB_HISTORY_RUNNING, 1,
                                          UMI_STATUS_OK, &out) == UMI_STATUS_BUSY &&
                      out.id == 99);
            }
            else if (strcmp(name, "backward") == 0)
            {
                CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 1,
                                          UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
                CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 0,
                                          UMI_STATUS_OK, &out) == UMI_STATUS_INVALID_STATE);
            }
            else if (strcmp(name, "lifecycle") == 0 || strcmp(name, "terminal") == 0)
            {
                CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_SUCCEEDED, 2,
                                          UMI_STATUS_OK, &entry) == UMI_STATUS_OK);
                CHECK(UmiJobHistoryCapture(history, snapshot) == UMI_STATUS_OK && snapshot->count == 1 &&
                      snapshot->unfinished_count == 0);
                CHECK(UmiJobHistoryUpdate(history, entry.id, entry.revision, UMI_JOB_HISTORY_RUNNING, 2,
                                          UMI_STATUS_OK, &out) == UMI_STATUS_INVALID_STATE);
            }
            else
                return 2;
        }
    }
    UmiJobHistoryDestroy(other);
    UmiJobHistoryDestroy(history);
    umi_data_server_destroy(server);
    free(snapshot);
    return 0;
}

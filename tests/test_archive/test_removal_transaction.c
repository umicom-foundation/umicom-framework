/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_removal_transaction.c
 * PURPOSE: Inject cancellation and storage refusal at the archive transaction boundary.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
/* Compile the transaction owner against narrow fixture wrappers. The underlying
 * memory Data Server still owns real begin/rollback behavior; only the selected
 * delete or commit boundary is intercepted. No production fault switch is added. */
static UmiCancellationToken *removal_cancel;
static const char *cancel_key;
static const char *refuse_key;
static bool refuse_commit;
static unsigned rollback_calls;
static UmiStatus RemovalDelete(UmiDataServer *server, const char *key)
{
    if (refuse_key != NULL && strcmp(key, refuse_key) == 0)
        return UMI_STATUS_IO_ERROR;
    UmiStatus status = umi_data_server_delete(server, key);
    if (status == UMI_STATUS_OK && cancel_key != NULL && strcmp(key, cancel_key) == 0)
        umi_cancellation_token_request(removal_cancel);
    return status;
}
static UmiStatus RemovalCommit(UmiDataServer *server)
{
    return refuse_commit ? UMI_STATUS_IO_ERROR : umi_data_server_commit(server);
}
static UmiStatus RemovalRollback(UmiDataServer *server)
{
    ++rollback_calls;
    return umi_data_server_rollback(server);
}
#define umi_data_server_delete RemovalDelete
#define umi_data_server_commit RemovalCommit
#define umi_data_server_rollback RemovalRollback
#include "../../src/testing/archive.c"
#undef umi_data_server_delete
#undef umi_data_server_commit
#undef umi_data_server_rollback
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiDataServer *server = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveOrigin origin = FixtureOrigin(true);
    UmiTestArchiveEntry saved = {0};
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &saved) == UMI_STATUS_OK);
    size_t before = umi_data_server_count(server);
    CHECK(umi_cancellation_token_create(&removal_cancel) == UMI_STATUS_OK);
    if (strcmp(mode, "cancel-before") == 0)
        umi_cancellation_token_request(removal_cancel);
    else if (strcmp(mode, "cancel-requests") == 0)
        cancel_key = "ctest-archive/test/run/1/request/0";
    else if (strcmp(mode, "cancel-publication") == 0)
        cancel_key = "ctest-archive/test/slot/0";
    else if (strcmp(mode, "delete-failure") == 0)
        refuse_key = "ctest-archive/test/run/1/request/1";
    else if (strcmp(mode, "commit-failure") == 0)
        refuse_commit = true;
    else
        CHECK(strcmp(mode, "success") == 0);
    UmiStatus status = UmiTestArchiveRemoveWithCancellation(archive, saved.id, removal_cancel);
    UmiStatus expected = strncmp(mode, "cancel-", 7) == 0  ? UMI_STATUS_CANCELLED
                         : strstr(mode, "failure") != NULL ? UMI_STATUS_IO_ERROR
                                                           : UMI_STATUS_OK;
    CHECK(status == expected && !umi_data_server_in_transaction(server));
    CHECK(rollback_calls ==
          (expected != UMI_STATUS_OK && strcmp(mode, "cancel-before") != 0 ? 1U : 0U));
    refuse_commit = false;
    refuse_key = NULL;
    cancel_key = NULL;
    if (expected != UMI_STATUS_OK)
    {
        CHECK(umi_data_server_count(server) == before);
        CHECK(UmiTestArchiveRead(archive, saved.id, &saved) == UMI_STATUS_OK);
        UmiTestArchiveAttempt *attempt = calloc(1, sizeof(*attempt));
        CHECK(attempt != NULL);
        for (size_t i = 0; i < 4; ++i)
        {
            CHECK(UmiTestArchiveReadAttempt(archive, saved.id, i, NULL, attempt) == UMI_STATUS_OK);
            CHECK(strcmp(attempt->result.output, job->results[i].output) == 0);
        }
        free(attempt);
        CHECK(UmiTestArchiveRemoveWithCancellation(archive, saved.id, NULL) == UMI_STATUS_OK);
    }
    CHECK(umi_data_server_count(server) == 1);
    FixtureEmpty(archive);
    umi_cancellation_token_destroy(removal_cancel);
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}

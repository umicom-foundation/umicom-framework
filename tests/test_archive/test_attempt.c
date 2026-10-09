/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_attempt.c
 * PURPOSE: Verify complete-attempt observations publish atomically or leave caller storage
 * unchanged. AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    UmiDataServer *server = NULL;
    UmiTestArchive *archive = NULL;
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveOrigin origin = FixtureOrigin(true);
    UmiTestArchiveEntry saved = {0};
    CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &saved) == UMI_STATUS_OK);
    UmiTestArchiveAttempt *out = calloc(1, sizeof(*out)), *before = calloc(1, sizeof(*before));
    CHECK(out != NULL && before != NULL);
    memset(out, 0x5a, sizeof(*out));
    *before = *out;
    UmiCancellationToken *token = NULL;
    CHECK(umi_cancellation_token_create(&token) == UMI_STATUS_OK);
    UmiStatus expected = UMI_STATUS_OK;
    uint64_t id = saved.id;
    size_t index = 3;
    if (strcmp(argv[1], "missing") == 0)
    {
        id = 99;
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(argv[1], "index") == 0)
    {
        index = 4;
        expected = UMI_STATUS_NOT_FOUND;
    }
    else if (strcmp(argv[1], "cancel") == 0)
    {
        umi_cancellation_token_request(token);
        expected = UMI_STATUS_CANCELLED;
    }
    else if (strcmp(argv[1], "corrupt") == 0)
    {
        CHECK(umi_data_server_delete(server, "ctest-archive/test/run/1/result/3/part/0") ==
              UMI_STATUS_OK);
        expected = UMI_STATUS_PARSE_ERROR;
    }
    else if (strcmp(argv[1], "transaction") == 0)
    {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        expected = UMI_STATUS_BUSY;
    }
    else
        CHECK(strcmp(argv[1], "complete") == 0);
    CHECK(UmiTestArchiveReadAttempt(archive, id, index, token, out) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(out->entry.id == saved.id && out->attempt == 2);
        CHECK(out->result.state == UMI_TEST_STATE_TIMED_OUT);
        CHECK(strcmp(out->result.test_id, out->request.test_id) == 0);
        CHECK(strcmp(out->result.output, job->results[3].output) == 0);
    }
    else
        CHECK(memcmp(out, before, sizeof(*out)) == 0);
    if (strcmp(argv[1], "transaction") == 0)
    {
        CHECK(umi_data_server_in_transaction(server));
        CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    }
    umi_cancellation_token_destroy(token);
    free(out);
    free(before);
    free(job);
    UmiTestArchiveDestroy(archive);
    umi_data_server_destroy(server);
    return 0;
}

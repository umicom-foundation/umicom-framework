/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_evidence.c
 * PURPOSE: Keep persisted test context, selection evidence and legacy decoding distinct.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/testing/archive_compare.h"
#include "umicom/testing/selection_identity.h"
static void SetIdentity(UmiJobIdentity *identity)
{
    memset(identity, 0, sizeof(*identity));
    memset(identity->subject, 'a', 64);
    memset(identity->configuration, 'b', 64);
    memset(identity->inputs, 'c', 64);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveOrigin origin = FixtureOrigin(false);
    SetIdentity(&origin.identity);
    UmiTestArchiveEntry entry = {0}, output = {0};
    entry.id = 1;
    entry.origin = origin;
    entry.plan = job->plan;
    entry.run = job->snapshot;
    char wire[4096];
    if (strcmp(mode, "legacy") == 0)
    {
        memset(&entry.origin.identity, 0, sizeof(entry.origin.identity));
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
        CHECK(strncmp(wire, "1|", 2) == 0);
        CHECK(UmiTestArchiveDecodeEntry(wire, &output) == UMI_STATUS_OK);
        CHECK(!UmiJobIdentityIsRecorded(&output.origin.identity) &&
              output.selection_digest[0] == '\0');
    }
    else if (strcmp(mode, "codec") == 0 || strcmp(mode, "truncated") == 0 ||
             strcmp(mode, "maximal") == 0)
    {
        CHECK(UmiTestSelectionDigest(&job->plan, job->requests, entry.selection_digest) ==
              UMI_STATUS_OK);
        if (strcmp(mode, "maximal") == 0)
        {
            memset(entry.origin.source_root, 'r', sizeof(entry.origin.source_root) - 1U);
            entry.origin.source_root[sizeof(entry.origin.source_root) - 1U] = '\0';
            memset(entry.origin.source_revision, 'v', sizeof(entry.origin.source_revision) - 1U);
            entry.origin.source_revision[sizeof(entry.origin.source_revision) - 1U] = '\0';
        }
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
        CHECK(strncmp(wire, "2|", 2) == 0 && strlen(wire) < 4096);
        CHECK(UmiTestArchiveDecodeEntry(wire, &output) == UMI_STATUS_OK);
        CHECK(memcmp(&output.origin.identity, &origin.identity, sizeof(origin.identity)) == 0);
        CHECK(strcmp(output.selection_digest, entry.selection_digest) == 0);
        if (strcmp(mode, "truncated") == 0)
        {
            /* Every prefix of an extended row is incomplete, including a prefix
             * that ends exactly where the old format used to finish. */
            size_t length = strlen(wire);
            for (size_t i = 0; i < length; ++i)
            {
                char saved = wire[i];
                wire[i] = '\0';
                output.id = 999;
                CHECK(UmiTestArchiveDecodeEntry(wire, &output) == UMI_STATUS_PARSE_ERROR);
                CHECK(output.id == 999);
                wire[i] = saved;
            }
        }
    }
    else if (strcmp(mode, "invalid") == 0)
    {
        entry.origin.identity.configuration[0] = '\0';
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, sizeof(wire)) == UMI_STATUS_INVALID_ARGUMENT);
        entry.origin = origin;
        memset(entry.selection_digest, 'a', sizeof(entry.selection_digest));
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, sizeof(wire)) == UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        UmiDataServer *server = NULL;
        UmiTestArchive *archive = NULL;
        CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveCreate(server, "test", &archive) == UMI_STATUS_OK);
        if (strcmp(mode, "invalid-save") == 0)
        {
            origin.identity.inputs[1] = 'Q';
            output.id = 999;
            CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &output) ==
                  UMI_STATUS_INVALID_ARGUMENT);
            CHECK(output.id == 999 && umi_data_server_count(server) == 0);
        }
        else
        {
            CHECK(UmiTestArchiveSave(archive, job, &origin, NULL, &entry) == UMI_STATUS_OK);
            CHECK(entry.selection_digest[0] != '\0');
            /* Alter the caller's context after saving; archive storage owns its copy. */
            origin.identity.configuration[0] = 'd';
            CHECK(UmiTestArchiveRead(archive, entry.id, &output) == UMI_STATUS_OK);
            CHECK(output.origin.identity.configuration[0] == 'b');
            CHECK(strcmp(output.selection_digest, entry.selection_digest) == 0);
            UmiStatus expected = UMI_STATUS_OK;
            if (strcmp(mode, "corrupt-selection") == 0)
            {
                entry.selection_digest[0] = entry.selection_digest[0] == 'a' ? 'b' : 'a';
                CHECK(UmiTestArchiveEncodeEntry(&entry, wire, sizeof(wire)) == UMI_STATUS_OK);
                CHECK(umi_data_server_set(server, "ctest-archive/test/slot/0", wire) ==
                      UMI_STATUS_OK);
                expected = UMI_STATUS_PARSE_ERROR;
            }
            else if (strcmp(mode, "corrupt-request") == 0)
            {
                ++job->requests[0].timeout_ms;
                CHECK(UmiTestArchiveEncodeRequest(&job->requests[0], wire, sizeof(wire)) ==
                      UMI_STATUS_OK);
                CHECK(umi_data_server_set(server, "ctest-archive/test/run/1/request/0", wire) ==
                      UMI_STATUS_OK);
                expected = UMI_STATUS_PARSE_ERROR;
            }
            else
                CHECK(strcmp(mode, "store") == 0);
            UmiTestArchiveComparison *comparison = NULL;
            CHECK(UmiTestArchiveCompare(archive, 1, 1, NULL, &comparison) == expected);
            if (expected != UMI_STATUS_OK)
                CHECK(comparison == NULL);
            else
            {
                UmiTestArchiveComparisonSummary summary = {0};
                CHECK(UmiTestArchiveComparisonRead(comparison, &summary) == UMI_STATUS_OK);
                CHECK(summary.identity_comparison == UMI_JOB_IDENTITY_SAME_RECORDED_INPUTS);
                CHECK(summary.same_recorded_selection && summary.selection_evidence_recorded);
            }
            UmiTestArchiveComparisonDestroy(comparison);
        }
        UmiTestArchiveDestroy(archive);
        umi_data_server_destroy(server);
    }
    free(job);
    return 0;
}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_archive/test_codec.c
 * PURPOSE: Reject malformed archive fields without partially publishing caller outputs.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include <limits.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *name = argv[1];
    UmiCtestJob *job = FixtureJob();
    UmiTestArchiveEntry entry = {0}, decoded = {0};
    entry.id = 1;
    entry.origin = FixtureOrigin(true);
    entry.plan = job->plan;
    entry.run = job->snapshot;
    char *wire = calloc(UMI_TEST_ARCHIVE_WIRE_CAPACITY, 1);
    CHECK(wire != NULL);
    if (strcmp(name, "entry") == 0)
    {
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) == UMI_STATUS_OK);
        CHECK(UmiTestArchiveDecodeEntry(wire, &decoded) == UMI_STATUS_OK);
        CHECK(decoded.id == 1 && decoded.run.timed_out == 1 && decoded.plan.repeat_count == 2 &&
              strcmp(decoded.origin.source_root, entry.origin.source_root) == 0 &&
              decoded.origin.retain_output);
    }
    else if (strcmp(name, "request") == 0)
    {
        UmiCtestJobRequest request = {0};
        CHECK(UmiTestArchiveEncodeRequest(&job->requests[0], wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) ==
              UMI_STATUS_OK);
        CHECK(UmiTestArchiveDecodeRequest(wire, &request) == UMI_STATUS_OK && request.timeout_ms == 100 &&
              strcmp(request.name, job->requests[0].name) == 0 && request.enabled == 1);
    }
    else if (strcmp(name, "exit-range") == 0 || strcmp(name, "result") == 0)
    {
        UmiTestResult *result = calloc(1, sizeof(*result));
        CHECK(result != NULL);
        job->results[2].exit_code = strcmp(name, "exit-range") == 0 ? INT_MIN : INT_MAX;
        CHECK(UmiTestArchiveEncodeResult(&job->results[2], wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) ==
              UMI_STATUS_OK);
        CHECK(UmiTestArchiveDecodeResult(wire, result) == UMI_STATUS_OK);
        CHECK(result->exit_code == job->results[2].exit_code &&
              strcmp(result->output, job->results[2].output) == 0);
        free(result);
    }
    else if (strcmp(name, "bounded") == 0)
    {
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, 8) == UMI_STATUS_CAPACITY_EXCEEDED);
    }
    else if (strcmp(name, "unterminated") == 0)
    {
        memset(entry.origin.source_root, 'x', sizeof(entry.origin.source_root));
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(name, "bad-count") == 0)
    {
        entry.run.failed = SIZE_MAX;
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(name, "invalid-enum") == 0)
    {
        job->results[0].status = (UmiStatus)-1;
        CHECK(UmiTestArchiveEncodeResult(&job->results[0], wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) ==
              UMI_STATUS_INVALID_ARGUMENT);
        job->results[0].status = UMI_STATUS_OK;
        job->results[0].state = (UmiTestState)-1;
        CHECK(UmiTestArchiveEncodeResult(&job->results[0], wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else if (strcmp(name, "false-pass") == 0)
    {
        job->results[0].status = UMI_STATUS_IO_ERROR;
        CHECK(UmiTestArchiveEncodeResult(&job->results[0], wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        CHECK(UmiTestArchiveEncodeEntry(&entry, wire, UMI_TEST_ARCHIVE_WIRE_CAPACITY) == UMI_STATUS_OK);
        if (strcmp(name, "format") == 0)
            wire[0] = '9';
        else if (strcmp(name, "truncated") == 0)
            wire[strlen(wire) - 1U] = '\0';
        else if (strcmp(name, "trailing") == 0)
            strcat(wire, "x");
        else if (strcmp(name, "overflow") == 0)
            strcpy(wire, "1|18446744073709551616|");
        else if (strcmp(name, "negative") == 0)
            strcpy(wire, "1|-1|");
        else if (strcmp(name, "nul") == 0)
        {
            /* Replace the final optional revision with an encoded embedded NUL. */
            char *last = strrchr(wire, '|');
            CHECK(last != NULL);
            *last = '\0';
            char *previous = strrchr(wire, '|');
            CHECK(previous != NULL);
            strcpy(previous + 1, "00|");
        }
        else
            CHECK(0);
        decoded.id = 999;
        CHECK(UmiTestArchiveDecodeEntry(wire, &decoded) == UMI_STATUS_PARSE_ERROR && decoded.id == 999);
    }
    free(wire);
    free(job);
    return 0;
}

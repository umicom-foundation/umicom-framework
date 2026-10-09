/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_working_tree/test_process.c
 * PURPOSE: Inject process outcomes and fragmented binary status without launching Git.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"

static const char *mode;
/* Replace only process execution in this executable. No test contacts a real repository or network.
 */
UmiStatus UmiProcessExecuteWithLifetime(const UmiProcessRequest *request,
                                        UmiProcessLifetime lifetime,
                                        UmiProcessResultObserver observer,
                                        UmiProcessOutputObserver raw, void *context,
                                        UmiProcessResult *out)
{
    static const char payload[] = HEADERS ORDINARY "café\nwith space.c\0";
    (void)observer;
    CHECK(strcmp(request->program, "chosen-git") == 0);
    CHECK(strcmp(request->working_directory, "repository with spaces") == 0);
    CHECK(request->capture_stdout && request->capture_stderr);
    CHECK(request->window_mode == UMI_PROCESS_WINDOW_HIDDEN &&
          lifetime == UMI_PROCESS_LIFETIME_TREE);
    CHECK(request->timeout_ms == 30000U && raw != NULL);
    CHECK(request->argument_count == 11U);
    CHECK(strcmp(request->arguments[0], "--no-optional-locks") == 0);
    CHECK(strcmp(request->arguments[2], "core.fsmonitor=false") == 0);
    CHECK(strcmp(request->arguments[4], "--porcelain=v2") == 0);
    CHECK(strcmp(request->arguments[5], "-z") == 0);
    CHECK(strcmp(request->arguments[9], "--ignore-submodules=none") == 0);
    CHECK(strcmp(request->arguments[10], "--ahead-behind") == 0);
    memset(out, 0, sizeof(*out));
    out->launched = 1;
    if (strcmp(mode, "launch-failure") == 0)
        return UMI_STATUS_IO_ERROR;
    if (strcmp(mode, "not-launched") == 0)
    {
        out->launched = 0;
        return UMI_STATUS_OK;
    }
    if (strcmp(mode, "exit-failure") == 0)
    {
        out->exit_code = 128;
        return UMI_STATUS_OK;
    }
    if (strcmp(mode, "timeout") == 0)
    {
        out->timed_out = 1;
        return UMI_STATUS_OK;
    }
    if (strcmp(mode, "cancel") == 0)
    {
        out->cancelled = 1;
        return UMI_STATUS_OK;
    }
    if (strcmp(mode, "empty-output") == 0)
        return UMI_STATUS_OK;
    if (strcmp(mode, "truncated-record") == 0)
    {
        raw(payload, sizeof(payload) - 2U, context);
        return UMI_STATUS_OK;
    }
    if (strcmp(mode, "overflow") == 0)
    {
        char chunk[4096] = {0};
        for (size_t index = 0U; index <= UMI_VCS_WORKING_TREE_BYTE_LIMIT / sizeof(chunk); ++index)
            raw(chunk, sizeof(chunk), context);
        return UMI_STATUS_OK;
    }
    /* Byte-sized chunks deliberately split UTF-8 sequences and the metadata fields. */
    for (size_t index = 0U; index < sizeof(payload) - 1U; ++index)
        raw(payload + index, 1U, context);
    if (strcmp(mode, "stderr-contamination") == 0)
        raw("warning: extra output\n", 22U, context);
    /* The fixed diagnostic tail may truncate while the complete raw stream remains intact. */
    out->output_truncated = 1;
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    mode = argv[1];
    UmiVcsWorkingTreeRequest request = {0};
    UmiVcsWorkingTree *sentinel = ParseFixture(NULL, 0U), *output = sentinel;
    request.repository_root = "repository with spaces";
    request.git_program = "chosen-git";
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "launch-failure") == 0)
        expected = UMI_STATUS_IO_ERROR;
    else if (strcmp(mode, "not-launched") == 0 || strcmp(mode, "exit-failure") == 0)
        expected = UMI_STATUS_UNAVAILABLE;
    else if (strcmp(mode, "timeout") == 0)
        expected = UMI_STATUS_TIMEOUT;
    else if (strcmp(mode, "cancel") == 0)
        expected = UMI_STATUS_CANCELLED;
    else if (strcmp(mode, "empty-output") == 0 || strcmp(mode, "truncated-record") == 0 ||
             strcmp(mode, "stderr-contamination") == 0)
        expected = UMI_STATUS_PARSE_ERROR;
    else if (strcmp(mode, "overflow") == 0)
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    else
        CHECK(strcmp(mode, "split-stream") == 0);
    CHECK(UmiVcsWorkingTreeRead(&request, &output) == expected);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(output != sentinel);
        CHECK(strcmp(UmiVcsWorkingTreeEntryAt(output, 0U)->change.path, "café\nwith space.c") == 0);
        UmiVcsWorkingTreeDestroy(output);
    }
    else
        CHECK(output == sentinel);
    UmiVcsWorkingTreeDestroy(sentinel);
    return 0;
}

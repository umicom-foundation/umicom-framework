/* Retain the direct-capture fixture for review. Maintenance now delegates
 * process handling to the existing VCS reader, tested through the seam below. */
#if 0
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_inspection/test_status_capture.c
 * PURPOSE: Keep failed, empty and incomplete Git status evidence out of shared UI projections.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/repository/status_probe.h"
#include "umicom/repository/git_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static const char *scenario;
static unsigned calls;
static int valid;
static UmiStatus FakeCapture(const UmiRepositoryInspectionContext *context,
    const char *const *arguments, size_t count, size_t limit,
    const UmiCancellationToken *cancellation, UmiRepositoryGitCaptureResult *out);
#define UmiRepositoryGitCapture FakeCapture
#include "../../src/repository/inspection/status_probe.c"
#undef UmiRepositoryGitCapture
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)

static UmiStatus FakeCapture(const UmiRepositoryInspectionContext *context,
    const char *const *arguments, size_t count, size_t limit,
    const UmiCancellationToken *cancellation, UmiRepositoryGitCaptureResult *out)
{
    ++calls;
    valid = context != NULL && count == 7U && limit == UMI_REPOSITORY_GIT_CAPTURE_LIMIT &&
        cancellation == NULL && strcmp(arguments[0], "--no-optional-locks") == 0 &&
        strcmp(arguments[1], "status") == 0 && strcmp(arguments[2], "--porcelain=v2") == 0 &&
        strcmp(arguments[3], "-z") == 0 && strcmp(arguments[4], "--branch") == 0 &&
        strcmp(arguments[5], "--untracked-files=all") == 0 &&
        strcmp(arguments[6], "--ignore-submodules=none") == 0;
    static const char good[] = "# branch.head main\0? file.c\0";
    static const char noBranch[] = "? file.c\0";
    static const char malformed[] = "# branch.head main\0? file.c";
    const char *bytes = strcmp(scenario, "missing_branch") == 0 ? noBranch :
        strcmp(scenario, "malformed") == 0 ? malformed : good;
    size_t length = strcmp(scenario, "missing_branch") == 0 ? sizeof noBranch - 1U :
        strcmp(scenario, "malformed") == 0 ? sizeof malformed - 1U : sizeof good - 1U;
    if (strcmp(scenario, "empty") == 0) length = 0U;
    memset(out, 0, sizeof *out);
    out->bytes = malloc(length + 1U);
    if (out->bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(out->bytes, bytes, length); out->bytes[length] = 0U; out->length = length;
    out->launched = 1;
    if (strcmp(scenario, "failed") == 0) { out->exit_code = 1; return UMI_STATUS_IO_ERROR; }
    if (strcmp(scenario, "truncated") == 0) { out->output_truncated = 1; return UMI_STATUS_CAPACITY_EXCEEDED; }
    return UMI_STATUS_OK;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    scenario = argv[1];
    UmiRepositoryInspectionContext context = {NULL, "unused", 1000U};
    UmiRepositoryPorcelainStatus value;
    memset(&value, 0x5a, sizeof value);
    if (strcmp(scenario, "null") == 0) {
        CHECK(umi_repository_status_probe_read(NULL, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_repository_status_probe_read(&context, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls == 0U && value.worktree.untracked == 0U);
        return 0;
    }
    UmiStatus expected = strcmp(scenario, "good") == 0 ? UMI_STATUS_OK :
        strcmp(scenario, "failed") == 0 ? UMI_STATUS_IO_ERROR :
        strcmp(scenario, "truncated") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_PARSE_ERROR;
    CHECK(umi_repository_status_probe_read(&context, &value) == expected);
    CHECK(calls == 1U && valid);
    if (expected == UMI_STATUS_OK)
        CHECK(strcmp(value.branch.branch, "main") == 0 && value.worktree.untracked == 1U);
    else {
        const unsigned char *bytes = (const unsigned char *)&value;
        for (size_t i = 0U; i < sizeof value; ++i) CHECK(bytes[i] == 0U);
    }
    return 0;
}

#endif

#include "umicom/repository/status_probe.h"
#include "umicom/vcs/working_tree.h"
#include <stdio.h>
#include <string.h>

static const char *scenario;
static unsigned calls;
static int valid;
static UmiStatus ReadObservation(const UmiVcsWorkingTreeRequest *request,
    UmiVcsWorkingTree **out_tree);
#define UmiVcsWorkingTreeRead ReadObservation
#include "../../src/repository/inspection/status_probe.c"
#undef UmiVcsWorkingTreeRead
#define CHECK(value) do { if (!(value)) { \
    (void)fprintf(stderr, "line %d: %s\n", __LINE__, #value); return 1; \
} } while (0)

/* Exercise the maintenance adapter without starting a process. The VCS reader's
 * own protocol tests cover header validation; this boundary must forward its
 * result and use the caller's explicit repository, executable and timeout. */
static UmiStatus ReadObservation(const UmiVcsWorkingTreeRequest *request,
    UmiVcsWorkingTree **out_tree)
{
    ++calls;
    valid = request != NULL && out_tree != NULL &&
        strcmp(request->repository_root, "unused") == 0 &&
        strcmp(request->git_program, "chosen-git") == 0 &&
        request->timeout_ms == 1000U && request->cancellation == NULL;
    if (strcmp(scenario, "failed") == 0) return UMI_STATUS_IO_ERROR;
    if (strcmp(scenario, "truncated") == 0) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (strcmp(scenario, "cancelled") == 0) return UMI_STATUS_CANCELLED;
    if (strcmp(scenario, "timeout") == 0) return UMI_STATUS_TIMEOUT;
    if (strcmp(scenario, "empty") == 0 || strcmp(scenario, "missing_branch") == 0)
        return UMI_STATUS_PARSE_ERROR;
    static const char good[] = "# branch.oid (initial)\0# branch.head main\0? file.c\0";
    static const char malformed[] = "# branch.head main\0? file.c";
    if (strcmp(scenario, "malformed") == 0)
        return UmiVcsWorkingTreeParse(malformed, sizeof malformed - 1U, out_tree);
    return UmiVcsWorkingTreeParse(good, sizeof good - 1U, out_tree);
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    scenario = argv[1];
    UmiRepositoryInspectionContext context = {"chosen-git", "unused", 1000U};
    UmiRepositoryPorcelainStatus value;
    memset(&value, 0x5a, sizeof value);
    if (strcmp(scenario, "null") == 0) {
        CHECK(umi_repository_status_probe_read(NULL, &value) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_repository_status_probe_read(&context, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(calls == 0U && value.worktree.untracked == 0U);
        return 0;
    }
    UmiStatus expected = strcmp(scenario, "good") == 0 ? UMI_STATUS_OK :
        strcmp(scenario, "failed") == 0 ? UMI_STATUS_IO_ERROR :
        strcmp(scenario, "truncated") == 0 ? UMI_STATUS_CAPACITY_EXCEEDED :
        strcmp(scenario, "cancelled") == 0 ? UMI_STATUS_CANCELLED :
        strcmp(scenario, "timeout") == 0 ? UMI_STATUS_TIMEOUT : UMI_STATUS_PARSE_ERROR;
    CHECK(umi_repository_status_probe_read(&context, &value) == expected);
    CHECK(calls == 1U && valid);
    if (expected == UMI_STATUS_OK)
        CHECK(strcmp(value.branch.branch, "main") == 0 && value.worktree.untracked == 1U);
    else {
        const unsigned char *bytes = (const unsigned char *)&value;
        for (size_t i = 0U; i < sizeof value; ++i) CHECK(bytes[i] == 0U);
    }
    return 0;
}

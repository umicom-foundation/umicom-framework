/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/status_probe.c
 *
 * PURPOSE:
 *   Implement the shared porcelain-v2 status probe used by repository inspection projections.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/status_probe.h"

#include "umicom/repository/git_command.h"
#include "umicom/repository/git_capture.h"
#include <string.h>

/* Execute one read-only status command and project its stable porcelain-v2 output. */
/* Preserve the text probe for review. Exact binary capture below avoids
 * quoted filenames, a large worker stack buffer and skipped malformed records. */
#if 0
UmiStatus umi_repository_status_probe_read(
    const UmiRepositoryInspectionContext *context,
    UmiRepositoryPorcelainStatus *out_status)
{
    const char *arguments[] = {
        "status",
        "--porcelain=v2",
        "--branch",
        "--untracked-files=all"
    };
    UmiRepositoryGitCommandResult result;
    UmiStatus status;

    /* Require caller-owned result storage before launching the Git process. */
    if (context == NULL || out_status == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Use the single safe argument-based Git execution boundary. */
    status = umi_repository_git_command_execute(
        context,
        arguments,
        sizeof(arguments) / sizeof(arguments[0]),
        &result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result.exit_code != 0) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Parse once so all derived status views share identical evidence. */
    return umi_repository_porcelain_parse(result.output, out_status);
}

#endif
/* The direct byte-capture probe is retained for review. The shared VCS reader
 * already owns status arguments, process lifetime and required header checks.
 * Maintenance now consumes that same observation instead of duplicating them. */
#if 0
UmiStatus umi_repository_status_probe_read(
    const UmiRepositoryInspectionContext *context,
    UmiRepositoryPorcelainStatus *out_status)
{
    if (out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof *out_status);
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Optional locks are disabled because observing status must not refresh
     * the index as a side effect. Submodule evidence is explicitly requested. */
    const char *arguments[] = {"--no-optional-locks", "status", "--porcelain=v2",
        "-z", "--branch", "--untracked-files=all", "--ignore-submodules=none"};
    UmiRepositoryGitCaptureResult result = {0};
    UmiStatus status = UmiRepositoryGitCapture(context, arguments,
        sizeof arguments / sizeof arguments[0], UMI_REPOSITORY_GIT_CAPTURE_LIMIT, NULL, &result);
    if (status == UMI_STATUS_OK)
        status = umi_repository_porcelain_parse_bytes(result.bytes, result.length, out_status);
    /* --branch always supplies a head label, including an unborn or detached
     * branch. An empty successful transcript is not proof of a clean checkout. */
    if (status == UMI_STATUS_OK && out_status->branch.branch[0] == '\0')
        status = UMI_STATUS_PARSE_ERROR;
    UmiRepositoryGitCaptureRelease(&result);
    if (status != UMI_STATUS_OK) memset(out_status, 0, sizeof *out_status);
    return status;
}

#endif

UmiStatus umi_repository_status_probe_read(
    const UmiRepositoryInspectionContext *context,
    UmiRepositoryPorcelainStatus *out_status)
{
    if (out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof *out_status);
    if (context == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Reuse Source Control's process policy and complete branch-header checks.
     * The observation owns its exact paths until projection finishes. */
    UmiVcsWorkingTreeRequest request = {
        .repository_root = context->repository_root, .git_program = context->git_program,
        .timeout_ms = context->timeout_ms
    };
    UmiVcsWorkingTree *tree = NULL;
    UmiStatus status = UmiVcsWorkingTreeRead(&request, &tree);
    if (status == UMI_STATUS_OK)
        status = UmiRepositoryPorcelainFromWorkingTree(tree, out_status);
    UmiVcsWorkingTreeDestroy(tree);
    return status;
}

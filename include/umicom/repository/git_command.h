/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/git_command.h
 *
 * PURPOSE:
 *   Define one safe argument-based Git execution boundary reused by every repository probe.
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

#ifndef UMICOM_REPOSITORY_GIT_COMMAND_H
#define UMICOM_REPOSITORY_GIT_COMMAND_H

#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository git command result data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryGitCommandResult {
    int exit_code;
    int launched;
    int timed_out;
    int cancelled;
    char output[UMI_REPOSITORY_INSPECTION_OUTPUT_CAPACITY];
    /* These flags describe incomplete evidence even when process execution fails.
     * Appending preserves earlier member offsets; rebuild callers of this record. */
    int output_truncated;
    int termination_requested;
} UmiRepositoryGitCommandResult;

/* Execute Git with argument vectors; shell command construction is deliberately avoided. */
/**
 * Capture both process evidence and its status. A nonzero Git exit remains a
 * failure status, but its exit code and diagnostic text are still available.
 * Truncated output is never returned as success for a caller to parse as a
 * complete repository snapshot. Invalid requests clear out_result when supplied
 * and set its exit_code to -1. This routine does not interpret Git subcommands.
 */
UmiStatus umi_repository_git_command_execute(
    const UmiRepositoryInspectionContext *context,
    const char *const *arguments,
    size_t argument_count,
    UmiRepositoryGitCommandResult *out_result);

#ifdef __cplusplus
}
#endif
#endif

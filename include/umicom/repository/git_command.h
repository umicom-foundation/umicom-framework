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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_REPOSITORY_GIT_COMMAND_H
#define UMICOM_REPOSITORY_GIT_COMMAND_H

#include <stddef.h>
#include "umicom/repository/inspection_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryGitCommandResult {
    int exit_code;
    int launched;
    int timed_out;
    int cancelled;
    char output[UMI_REPOSITORY_INSPECTION_OUTPUT_CAPACITY];
} UmiRepositoryGitCommandResult;

/* Execute Git with argument vectors; shell command construction is deliberately avoided. */
UmiStatus umi_repository_git_command_execute(
    const UmiRepositoryInspectionContext *context,
    const char *const *arguments,
    size_t argument_count,
    UmiRepositoryGitCommandResult *out_result);

#ifdef __cplusplus
}
#endif
#endif

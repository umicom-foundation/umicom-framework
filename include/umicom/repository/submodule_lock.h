/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/submodule_lock.h
 *
 * PURPOSE:
 *   Provide the native Framework-owned contract used to refresh a parent Git
 *   repository's submodule gitlinks from the checked-out submodule HEADs.
 *
 * ARCHITECTURE:
 *   This replaces repository-version PowerShell automation with a reusable C23
 *   capability.  The operation reads .gitmodules, validates that each path is
 *   a Git submodule, resolves its current HEAD through argument-based Git
 *   process execution, and stages only the parent gitlink.  It never commits
 *   and never pushes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_SUBMODULE_LOCK_H
#define UMICOM_REPOSITORY_SUBMODULE_LOCK_H

#include <stddef.h>

#include "umicom/repository/repository.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_SUBMODULE_LOCK_CAPACITY 64U
#define UMI_REPOSITORY_GIT_SHA_CAPACITY 65U

typedef struct UmiRepositorySubmoduleLockEntry {
    char name[UMI_REPOSITORY_TEXT_CAPACITY];
    char path[UMI_REPOSITORY_PATH_CAPACITY];
    char head[UMI_REPOSITORY_GIT_SHA_CAPACITY];
    int staged;
} UmiRepositorySubmoduleLockEntry;

typedef struct UmiRepositorySubmoduleLockRequest {
    const char *repository_root;
    int dry_run;
} UmiRepositorySubmoduleLockRequest;

typedef struct UmiRepositorySubmoduleLockReport {
    UmiRepositorySubmoduleLockEntry entries[
        UMI_REPOSITORY_SUBMODULE_LOCK_CAPACITY];
    size_t count;
    size_t locked_count;
    size_t staged_count;
    int last_exit_code;
    char last_output[UMI_PROCESS_OUTPUT_CAPACITY];
} UmiRepositorySubmoduleLockReport;

/*
 * Refresh the parent repository's submodule gitlinks from the submodules that
 * are already checked out on disk.
 *
 * dry_run != 0 performs all discovery and validation but does not stage.
 * The function never creates commits and never pushes remote state.
 */
UmiStatus umi_repository_submodule_lock(
    const UmiToolchainProfile *profile,
    UmiEnvironmentPlan *environment,
    const UmiRepositorySubmoduleLockRequest *request,
    UmiRepositorySubmoduleLockReport *out_report);

#ifdef __cplusplus
}
#endif

#endif

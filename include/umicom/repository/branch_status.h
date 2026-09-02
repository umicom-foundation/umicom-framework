/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/branch_status.h
 *
 * PURPOSE:
 *   Represent current branch/upstream/divergence state independently of a UI toolkit.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_BRANCH_STATUS_H
#define INCLUDE_UMICOM_REPOSITORY_BRANCH_STATUS_H
#include <stddef.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_BRANCH_NAME_CAPACITY 256U

/**
 * Represent the repository branch status data shared with callers of this public contract.
 */
typedef struct UmiRepositoryBranchStatus {
    char branch[UMI_REPOSITORY_BRANCH_NAME_CAPACITY];
    char upstream[UMI_REPOSITORY_BRANCH_NAME_CAPACITY];
    size_t ahead;
    size_t behind;
    int detached;
} UmiRepositoryBranchStatus;

/**
 * Initialise repository branch status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_branch_status_init(UmiRepositoryBranchStatus *status);
/**
 * Copy repository branch status into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_repository_branch_status_set(UmiRepositoryBranchStatus *status,
                                           const char *branch,
                                           const char *upstream,
                                           size_t ahead,
                                           size_t behind,
                                           int detached);
/**
 * Check that repository branch status satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_branch_status_validate(const UmiRepositoryBranchStatus *status);

#ifdef __cplusplus
}
#endif
#endif

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

typedef struct UmiRepositoryBranchStatus {
    char branch[UMI_REPOSITORY_BRANCH_NAME_CAPACITY];
    char upstream[UMI_REPOSITORY_BRANCH_NAME_CAPACITY];
    size_t ahead;
    size_t behind;
    int detached;
} UmiRepositoryBranchStatus;

void umi_repository_branch_status_init(UmiRepositoryBranchStatus *status);
UmiStatus umi_repository_branch_status_set(UmiRepositoryBranchStatus *status,
                                           const char *branch,
                                           const char *upstream,
                                           size_t ahead,
                                           size_t behind,
                                           int detached);
UmiStatus umi_repository_branch_status_validate(const UmiRepositoryBranchStatus *status);

#ifdef __cplusplus
}
#endif
#endif

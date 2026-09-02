/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/porcelain_parser.h
 *
 * PURPOSE:
 *   Parse one Git porcelain-v2 status stream into existing maintenance models.
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

#ifndef INCLUDE_UMICOM_REPOSITORY_PORCELAIN_PARSER_H
#define INCLUDE_UMICOM_REPOSITORY_PORCELAIN_PARSER_H
#include "umicom/repository/inspection_types.h"
#include "umicom/repository/branch_status.h"
#include "umicom/repository/worktree_status.h"
#include "umicom/repository/index_status.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository porcelain status data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryPorcelainStatus {
    UmiRepositoryBranchStatus branch;
    UmiRepositoryWorktreeStatus worktree;
    UmiRepositoryIndexStatus index;
    size_t submodule_dirty;
} UmiRepositoryPorcelainStatus;
/* Parse branch headers and path records from one status --porcelain=v2 stream. */
UmiStatus umi_repository_porcelain_parse(const char *text, UmiRepositoryPorcelainStatus *out_status);

#ifdef __cplusplus
}
#endif
#endif

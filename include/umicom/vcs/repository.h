/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/repository.h
 *
 * PURPOSE:
 *   Bind one repository root to a source-control provider and expose safe product operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_REPOSITORY_H
#define UMICOM_VCS_REPOSITORY_H

#include <stddef.h>

#include "umicom/base/status.h"
#include "umicom/vcs/provider.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs repository data shared with callers of this public contract.
 */
typedef struct UmiVcsRepository UmiVcsRepository;

/**
 * Initialise vcs repository from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_repository_create(const char *root,
                                    const UmiVcsProvider *provider,
                                    UmiVcsRepository **out_repository);
/**
 * Release or reset state held by vcs repository so the same storage can be reused safely.
 */
void umi_vcs_repository_destroy(UmiVcsRepository *repository);
/**
 * Provide the vcs repository root operation used by this module and its client
 * applications.
 */
const char *umi_vcs_repository_root(const UmiVcsRepository *repository);
/**
 * Provide the vcs repository provider id operation used by this module and its client
 * applications.
 */
const char *umi_vcs_repository_provider_id(
    const UmiVcsRepository *repository
);
/**
 * Provide the vcs repository capabilities operation used by this module and its client
 * applications.
 */
uint64_t umi_vcs_repository_capabilities(const UmiVcsRepository *repository);
/**
 * Provide the vcs repository status operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_status(UmiVcsRepository *repository,
                                    UmiVcsChangeList *out_changes,
                                    UmiVcsBranch *out_branch);
/**
 * Provide the vcs repository history operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_history(UmiVcsRepository *repository,
                                     size_t limit,
                                     UmiVcsHistory *out_history);
/**
 * Provide the vcs repository stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_stage(UmiVcsRepository *repository,
                                   const char *path);
/**
 * Provide the vcs repository unstage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_unstage(UmiVcsRepository *repository,
                                     const char *path);
/**
 * Provide the vcs repository commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_commit(UmiVcsRepository *repository,
                                    const char *message,
                                    char *out_commit_id,
                                    size_t capacity);
/**
 * Provide the vcs repository pull operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_pull(UmiVcsRepository *repository);
/**
 * Provide the vcs repository push operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_push(UmiVcsRepository *repository);
/**
 * Provide the vcs repository branches operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_branches(UmiVcsRepository *repository,
                                      UmiVcsBranchList *out_branches);
/**
 * Provide the vcs repository remotes operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_remotes(UmiVcsRepository *repository,
                                     UmiVcsRemoteList *out_remotes);
/**
 * Provide the vcs repository tags operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_tags(UmiVcsRepository *repository,
                                  UmiVcsTagList *out_tags);
/**
 * Provide the vcs repository diff operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_diff(UmiVcsRepository *repository,
                                  const char *path,
                                  int staged,
                                  char *out_text,
                                  size_t capacity);
/**
 * Provide the vcs repository stage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_stage_all(UmiVcsRepository *repository);
/**
 * Provide the vcs repository unstage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_unstage_all(UmiVcsRepository *repository);
/**
 * Provide the vcs repository discard operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_discard(UmiVcsRepository *repository,
                                     const char *path);
/**
 * Provide the vcs repository fetch operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_fetch(UmiVcsRepository *repository);
/**
 * Initialise vcs repository branch from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_vcs_repository_branch_create(UmiVcsRepository *repository,
                                           const char *name,
                                           int checkout);
/**
 * Provide the vcs repository branch checkout operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_branch_checkout(UmiVcsRepository *repository,
                                             const char *name);
/**
 * Provide the vcs repository branch delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_branch_delete(UmiVcsRepository *repository,
                                           const char *name,
                                           int force);

#ifdef __cplusplus
}
#endif

#endif

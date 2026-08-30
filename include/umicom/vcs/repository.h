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

typedef struct UmiVcsRepository UmiVcsRepository;

UmiStatus umi_vcs_repository_create(const char *root,
                                    const UmiVcsProvider *provider,
                                    UmiVcsRepository **out_repository);
void umi_vcs_repository_destroy(UmiVcsRepository *repository);
const char *umi_vcs_repository_root(const UmiVcsRepository *repository);
const char *umi_vcs_repository_provider_id(
    const UmiVcsRepository *repository
);
uint64_t umi_vcs_repository_capabilities(const UmiVcsRepository *repository);
UmiStatus umi_vcs_repository_status(UmiVcsRepository *repository,
                                    UmiVcsChangeList *out_changes,
                                    UmiVcsBranch *out_branch);
UmiStatus umi_vcs_repository_history(UmiVcsRepository *repository,
                                     size_t limit,
                                     UmiVcsHistory *out_history);
UmiStatus umi_vcs_repository_stage(UmiVcsRepository *repository,
                                   const char *path);
UmiStatus umi_vcs_repository_unstage(UmiVcsRepository *repository,
                                     const char *path);
UmiStatus umi_vcs_repository_commit(UmiVcsRepository *repository,
                                    const char *message,
                                    char *out_commit_id,
                                    size_t capacity);
UmiStatus umi_vcs_repository_pull(UmiVcsRepository *repository);
UmiStatus umi_vcs_repository_push(UmiVcsRepository *repository);
UmiStatus umi_vcs_repository_branches(UmiVcsRepository *repository,
                                      UmiVcsBranchList *out_branches);
UmiStatus umi_vcs_repository_remotes(UmiVcsRepository *repository,
                                     UmiVcsRemoteList *out_remotes);
UmiStatus umi_vcs_repository_tags(UmiVcsRepository *repository,
                                  UmiVcsTagList *out_tags);
UmiStatus umi_vcs_repository_diff(UmiVcsRepository *repository,
                                  const char *path,
                                  int staged,
                                  char *out_text,
                                  size_t capacity);
UmiStatus umi_vcs_repository_stage_all(UmiVcsRepository *repository);
UmiStatus umi_vcs_repository_unstage_all(UmiVcsRepository *repository);
UmiStatus umi_vcs_repository_discard(UmiVcsRepository *repository,
                                     const char *path);
UmiStatus umi_vcs_repository_fetch(UmiVcsRepository *repository);
UmiStatus umi_vcs_repository_branch_create(UmiVcsRepository *repository,
                                           const char *name,
                                           int checkout);
UmiStatus umi_vcs_repository_branch_checkout(UmiVcsRepository *repository,
                                             const char *name);
UmiStatus umi_vcs_repository_branch_delete(UmiVcsRepository *repository,
                                           const char *name,
                                           int force);

#ifdef __cplusplus
}
#endif

#endif

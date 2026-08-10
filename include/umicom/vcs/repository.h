/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/repository.h
 *
 * PURPOSE:
 *   Bind one repository root to a source-control provider and expose safe product operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

#ifdef __cplusplus
}
#endif

#endif

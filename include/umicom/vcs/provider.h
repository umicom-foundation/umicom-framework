/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/provider.h
 *
 * PURPOSE:
 *   Define the provider-neutral source-control operations used by Framework products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_PROVIDER_H
#define UMICOM_VCS_PROVIDER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/vcs/branch.h"
#include "umicom/vcs/change.h"
#include "umicom/vcs/history.h"
#include "umicom/vcs/branch_list.h"
#include "umicom/vcs/remote.h"
#include "umicom/vcs/tag.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsProvider {
    uint32_t structure_size;
    const char *provider_id;
    void *instance;
    UmiStatus (*status)(void *instance,
                        const char *root,
                        UmiVcsChangeList *out_changes,
                        UmiVcsBranch *out_branch);
    UmiStatus (*history)(void *instance,
                         const char *root,
                         size_t limit,
                         UmiVcsHistory *out_history);
    UmiStatus (*stage)(void *instance,
                       const char *root,
                       const char *path);
    UmiStatus (*unstage)(void *instance,
                         const char *root,
                         const char *path);
    UmiStatus (*commit)(void *instance,
                        const char *root,
                        const char *message,
                        char *out_commit_id,
                        size_t capacity);
    UmiStatus (*pull)(void *instance,
                      const char *root);
    UmiStatus (*push)(void *instance,
                      const char *root);
    void (*destroy)(void *instance);
    /* Contract v2 capabilities are appended to preserve the v1 ABI prefix. */
    UmiStatus (*branches)(void *instance,
                          const char *root,
                          UmiVcsBranchList *out_branches);
    UmiStatus (*remotes)(void *instance,
                         const char *root,
                         UmiVcsRemoteList *out_remotes);
    UmiStatus (*tags)(void *instance,
                      const char *root,
                      UmiVcsTagList *out_tags);
    UmiStatus (*diff)(void *instance,
                      const char *root,
                      const char *path,
                      int staged,
                      char *out_text,
                      size_t capacity);
    UmiStatus (*stage_all)(void *instance, const char *root);
    UmiStatus (*unstage_all)(void *instance, const char *root);
    UmiStatus (*discard)(void *instance,
                         const char *root,
                         const char *path);
    UmiStatus (*fetch)(void *instance, const char *root);
    UmiStatus (*branch_create)(void *instance,
                               const char *root,
                               const char *name,
                               int checkout);
    UmiStatus (*branch_checkout)(void *instance,
                                 const char *root,
                                 const char *name);
    UmiStatus (*branch_delete)(void *instance,
                               const char *root,
                               const char *name,
                               int force);
} UmiVcsProvider;

UmiStatus umi_vcs_provider_validate(const UmiVcsProvider *provider);
uint64_t umi_vcs_provider_capabilities(const UmiVcsProvider *provider);

#ifdef __cplusplus
}
#endif

#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/repository.c
 *
 * PURPOSE:
 *   Implement repository-root ownership and provider dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/repository.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiVcsRepository {
    char root[UMI_VCS_PATH_CAPACITY];
    UmiVcsProvider provider;
};

/*
 * Initialise vcs repository from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_repository_create(const char *root,
                                    const UmiVcsProvider *provider,
                                    UmiVcsRepository **out_repository)
{
    UmiVcsRepository *repository;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || provider == NULL || out_repository == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_vcs_provider_validate(provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (strlen(root) + 1U > UMI_VCS_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    repository = calloc(1U, sizeof(*repository));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    (void)snprintf(repository->root,
                   sizeof(repository->root),
                   "%s",
                   root);
    repository->provider = *provider;
    *out_repository = repository;
    return UMI_STATUS_OK;
}

/* Release or reset state held by vcs repository so the same storage can be reused safely. */
void umi_vcs_repository_destroy(UmiVcsRepository *repository)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository != NULL) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (repository->provider.destroy != NULL) {
            repository->provider.destroy(repository->provider.instance);
        }
        free(repository);
    }
}

/*
 * Provide the vcs repository root operation used by this module and its client
 * applications.
 */
const char *umi_vcs_repository_root(const UmiVcsRepository *repository)
{
    return repository != NULL ? repository->root : NULL;
}

/*
 * Provide the vcs repository provider id operation used by this module and its client
 * applications.
 */
const char *umi_vcs_repository_provider_id(
    const UmiVcsRepository *repository)
{
    return repository != NULL
        ? repository->provider.provider_id
        : NULL;
}

/*
 * Provide the vcs repository capabilities operation used by this module and its client
 * applications.
 */
uint64_t umi_vcs_repository_capabilities(const UmiVcsRepository *repository)
{
    return repository != NULL
        ? umi_vcs_provider_capabilities(&repository->provider) : 0U;
}

/*
 * Provide the vcs repository status operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_status(UmiVcsRepository *repository,
                                    UmiVcsChangeList *out_changes,
                                    UmiVcsBranch *out_branch)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.status(repository->provider.instance,
                                       repository->root,
                                       out_changes,
                                       out_branch);
}

/*
 * Provide the vcs repository history operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_history(UmiVcsRepository *repository,
                                     size_t limit,
                                     UmiVcsHistory *out_history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.history(repository->provider.instance,
                                        repository->root,
                                        limit,
                                        out_history);
}

/*
 * Provide the vcs repository stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_stage(UmiVcsRepository *repository,
                                   const char *path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.stage(repository->provider.instance,
                                      repository->root,
                                      path);
}

/*
 * Provide the vcs repository unstage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_unstage(UmiVcsRepository *repository,
                                     const char *path)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.unstage(repository->provider.instance,
                                        repository->root,
                                        path);
}

/*
 * Provide the vcs repository commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_commit(UmiVcsRepository *repository,
                                    const char *message,
                                    char *out_commit_id,
                                    size_t capacity)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.commit(repository->provider.instance,
                                       repository->root,
                                       message,
                                       out_commit_id,
                                       capacity);
}

/*
 * Provide the vcs repository pull operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_pull(UmiVcsRepository *repository)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || repository->provider.pull == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return repository->provider.pull(repository->provider.instance,
                                     repository->root);
}

/*
 * Provide the vcs repository push operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_push(UmiVcsRepository *repository)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (repository == NULL || repository->provider.push == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return repository->provider.push(repository->provider.instance,
                                     repository->root);
}

#define UMI_VCS_OPTIONAL_MEMBER(repository_, member_) \
    ((repository_) != NULL && \
     (repository_)->provider.structure_size >= \
         (uint32_t)(offsetof(UmiVcsProvider, member_) + \
                    sizeof((repository_)->provider.member_)) && \
     (repository_)->provider.member_ != NULL)

/*
 * Provide the vcs repository branches operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_branches(UmiVcsRepository *repository,
                                      UmiVcsBranchList *out_branches)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, branches)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.branches(repository->provider.instance, repository->root, out_branches);
}

/*
 * Provide the vcs repository remotes operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_remotes(UmiVcsRepository *repository,
                                     UmiVcsRemoteList *out_remotes)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, remotes)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.remotes(repository->provider.instance, repository->root, out_remotes);
}

/*
 * Provide the vcs repository tags operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_tags(UmiVcsRepository *repository,
                                  UmiVcsTagList *out_tags)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, tags)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.tags(repository->provider.instance, repository->root, out_tags);
}

/*
 * Provide the vcs repository diff operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_diff(UmiVcsRepository *repository,
                                  const char *path,
                                  int staged,
                                  char *out_text,
                                  size_t capacity)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, diff)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.diff(repository->provider.instance, repository->root, path, staged, out_text, capacity);
}

/*
 * Provide the vcs repository stage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_stage_all(UmiVcsRepository *repository)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, stage_all)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.stage_all(repository->provider.instance, repository->root);
}

/*
 * Provide the vcs repository unstage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_unstage_all(UmiVcsRepository *repository)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, unstage_all)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.unstage_all(repository->provider.instance, repository->root);
}

/*
 * Provide the vcs repository discard operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_discard(UmiVcsRepository *repository, const char *path)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, discard)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.discard(repository->provider.instance, repository->root, path);
}

/*
 * Provide the vcs repository fetch operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_fetch(UmiVcsRepository *repository)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, fetch)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.fetch(repository->provider.instance, repository->root);
}

/*
 * Initialise vcs repository branch from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_vcs_repository_branch_create(UmiVcsRepository *repository, const char *name, int checkout)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, branch_create)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.branch_create(repository->provider.instance, repository->root, name, checkout);
}

/*
 * Provide the vcs repository branch checkout operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_branch_checkout(UmiVcsRepository *repository, const char *name)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, branch_checkout)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.branch_checkout(repository->provider.instance, repository->root, name);
}

/*
 * Provide the vcs repository branch delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_repository_branch_delete(UmiVcsRepository *repository, const char *name, int force)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_OPTIONAL_MEMBER(repository, branch_delete)) return UMI_STATUS_NOT_IMPLEMENTED;
    return repository->provider.branch_delete(repository->provider.instance, repository->root, name, force);
}

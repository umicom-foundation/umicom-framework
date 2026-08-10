/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/repository.c
 *
 * PURPOSE:
 *   Implement repository-root ownership and provider dispatch.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/repository.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiVcsRepository {
    char root[UMI_VCS_PATH_CAPACITY];
    UmiVcsProvider provider;
};

UmiStatus umi_vcs_repository_create(const char *root,
                                    const UmiVcsProvider *provider,
                                    UmiVcsRepository **out_repository)
{
    UmiVcsRepository *repository;
    UmiStatus status;
    if (root == NULL || provider == NULL || out_repository == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_vcs_provider_validate(provider);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    if (strlen(root) + 1U > UMI_VCS_PATH_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    repository = calloc(1U, sizeof(*repository));
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

void umi_vcs_repository_destroy(UmiVcsRepository *repository)
{
    if (repository != NULL) {
        if (repository->provider.destroy != NULL) {
            repository->provider.destroy(repository->provider.instance);
        }
        free(repository);
    }
}

const char *umi_vcs_repository_root(const UmiVcsRepository *repository)
{
    return repository != NULL ? repository->root : NULL;
}

const char *umi_vcs_repository_provider_id(
    const UmiVcsRepository *repository)
{
    return repository != NULL
        ? repository->provider.provider_id
        : NULL;
}

UmiStatus umi_vcs_repository_status(UmiVcsRepository *repository,
                                    UmiVcsChangeList *out_changes,
                                    UmiVcsBranch *out_branch)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.status(repository->provider.instance,
                                       repository->root,
                                       out_changes,
                                       out_branch);
}

UmiStatus umi_vcs_repository_history(UmiVcsRepository *repository,
                                     size_t limit,
                                     UmiVcsHistory *out_history)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.history(repository->provider.instance,
                                        repository->root,
                                        limit,
                                        out_history);
}

UmiStatus umi_vcs_repository_stage(UmiVcsRepository *repository,
                                   const char *path)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.stage(repository->provider.instance,
                                      repository->root,
                                      path);
}

UmiStatus umi_vcs_repository_unstage(UmiVcsRepository *repository,
                                     const char *path)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.unstage(repository->provider.instance,
                                        repository->root,
                                        path);
}

UmiStatus umi_vcs_repository_commit(UmiVcsRepository *repository,
                                    const char *message,
                                    char *out_commit_id,
                                    size_t capacity)
{
    if (repository == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return repository->provider.commit(repository->provider.instance,
                                       repository->root,
                                       message,
                                       out_commit_id,
                                       capacity);
}

UmiStatus umi_vcs_repository_pull(UmiVcsRepository *repository)
{
    if (repository == NULL || repository->provider.pull == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return repository->provider.pull(repository->provider.instance,
                                     repository->root);
}

UmiStatus umi_vcs_repository_push(UmiVcsRepository *repository)
{
    if (repository == NULL || repository->provider.push == NULL) {
        return UMI_STATUS_NOT_IMPLEMENTED;
    }
    return repository->provider.push(repository->provider.instance,
                                     repository->root);
}

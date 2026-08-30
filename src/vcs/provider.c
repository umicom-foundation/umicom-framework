/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/provider.c
 *
 * PURPOSE:
 *   Validate required source-control provider operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/provider.h"

#include <stddef.h>

UmiStatus umi_vcs_provider_validate(const UmiVcsProvider *provider)
{
    if (provider == NULL ||
        provider->structure_size <
            (uint32_t)(offsetof(UmiVcsProvider, destroy) +
                       sizeof(provider->destroy)) ||
        provider->provider_id == NULL ||
        provider->provider_id[0] == '\0' ||
        provider->status == NULL ||
        provider->history == NULL ||
        provider->stage == NULL ||
        provider->unstage == NULL ||
        provider->commit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

#define UMI_VCS_PROVIDER_HAS(provider_, member_) \
    ((provider_) != NULL && \
     (provider_)->structure_size >= \
         (uint32_t)(offsetof(UmiVcsProvider, member_) + \
                    sizeof((provider_)->member_)) && \
     (provider_)->member_ != NULL)

uint64_t umi_vcs_provider_capabilities(const UmiVcsProvider *provider)
{
    uint64_t capabilities = 0U;
    if (provider == NULL) return 0U;
    if (provider->status != NULL) capabilities |= UMI_VCS_CAPABILITY_STATUS;
    if (provider->history != NULL) capabilities |= UMI_VCS_CAPABILITY_HISTORY;
    if (provider->stage != NULL && provider->unstage != NULL) capabilities |= UMI_VCS_CAPABILITY_STAGE;
    if (provider->commit != NULL) capabilities |= UMI_VCS_CAPABILITY_COMMIT;
    if (provider->pull != NULL) capabilities |= UMI_VCS_CAPABILITY_PULL;
    if (provider->push != NULL) capabilities |= UMI_VCS_CAPABILITY_PUSH;
    if (UMI_VCS_PROVIDER_HAS(provider, branches)) capabilities |= UMI_VCS_CAPABILITY_BRANCHES;
    if (UMI_VCS_PROVIDER_HAS(provider, remotes)) capabilities |= UMI_VCS_CAPABILITY_REMOTES;
    if (UMI_VCS_PROVIDER_HAS(provider, tags)) capabilities |= UMI_VCS_CAPABILITY_TAGS;
    if (UMI_VCS_PROVIDER_HAS(provider, diff)) capabilities |= UMI_VCS_CAPABILITY_DIFF;
    if (UMI_VCS_PROVIDER_HAS(provider, stage_all) && UMI_VCS_PROVIDER_HAS(provider, unstage_all)) capabilities |= UMI_VCS_CAPABILITY_STAGE_ALL;
    if (UMI_VCS_PROVIDER_HAS(provider, discard)) capabilities |= UMI_VCS_CAPABILITY_DISCARD;
    if (UMI_VCS_PROVIDER_HAS(provider, fetch)) capabilities |= UMI_VCS_CAPABILITY_FETCH;
    if (UMI_VCS_PROVIDER_HAS(provider, branch_create)) capabilities |= UMI_VCS_CAPABILITY_BRANCH_CREATE;
    if (UMI_VCS_PROVIDER_HAS(provider, branch_checkout)) capabilities |= UMI_VCS_CAPABILITY_BRANCH_CHECKOUT;
    if (UMI_VCS_PROVIDER_HAS(provider, branch_delete)) capabilities |= UMI_VCS_CAPABILITY_BRANCH_DELETE;
    return capabilities;
}

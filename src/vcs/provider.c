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

/* Check that vcs provider satisfies its contract before another service relies on it. */
UmiStatus umi_vcs_provider_validate(const UmiVcsProvider *provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

/*
 * Provide the vcs provider capabilities operation used by this module and its client
 * applications.
 */
uint64_t umi_vcs_provider_capabilities(const UmiVcsProvider *provider)
{
    uint64_t capabilities = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL) return 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->status != NULL) capabilities |= UMI_VCS_CAPABILITY_STATUS;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->history != NULL) capabilities |= UMI_VCS_CAPABILITY_HISTORY;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->stage != NULL && provider->unstage != NULL) capabilities |= UMI_VCS_CAPABILITY_STAGE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->commit != NULL) capabilities |= UMI_VCS_CAPABILITY_COMMIT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->pull != NULL) capabilities |= UMI_VCS_CAPABILITY_PULL;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider->push != NULL) capabilities |= UMI_VCS_CAPABILITY_PUSH;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, branches)) capabilities |= UMI_VCS_CAPABILITY_BRANCHES;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, remotes)) capabilities |= UMI_VCS_CAPABILITY_REMOTES;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, tags)) capabilities |= UMI_VCS_CAPABILITY_TAGS;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, diff)) capabilities |= UMI_VCS_CAPABILITY_DIFF;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, stage_all) && UMI_VCS_PROVIDER_HAS(provider, unstage_all)) capabilities |= UMI_VCS_CAPABILITY_STAGE_ALL;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, discard)) capabilities |= UMI_VCS_CAPABILITY_DISCARD;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, fetch)) capabilities |= UMI_VCS_CAPABILITY_FETCH;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, branch_create)) capabilities |= UMI_VCS_CAPABILITY_BRANCH_CREATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, branch_checkout)) capabilities |= UMI_VCS_CAPABILITY_BRANCH_CHECKOUT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (UMI_VCS_PROVIDER_HAS(provider, branch_delete)) capabilities |= UMI_VCS_CAPABILITY_BRANCH_DELETE;
    return capabilities;
}

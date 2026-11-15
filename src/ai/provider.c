/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/provider.c
 *
 * PURPOSE:
 *   Validate the stable AI-provider function table before registration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Validation prevents an incomplete or incompatible provider from being activated simply because a binary was discovered.
 */

#include "umicom/ai/provider.h"
#include <stddef.h>

/* Check that ai provider satisfies its contract before another service relies on it. */
UmiStatus umi_ai_provider_validate(const UmiAiProvider *provider)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
/* Provider kind is part of the dispatch contract. Reject unknown values before they can inherit local policy; retain the earlier ABI and callback validation for review. The former implementation is retained for owner review. */
#if 0
    if (provider == NULL || provider->structure_size < sizeof(*provider) ||
        provider->abi_version != 1U || provider->provider_id == NULL ||
        provider->provider_id[0] == '\0' || provider->generate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
#endif
    if (provider == NULL || provider->structure_size < sizeof(*provider) ||
        provider->abi_version != 1U || provider->provider_id == NULL ||
        provider->provider_id[0] == '\0' || provider->generate == NULL ||
        provider->kind < UMI_AI_PROVIDER_LOCAL || provider->kind > UMI_AI_PROVIDER_TEST) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

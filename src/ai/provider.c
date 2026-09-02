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
    if (provider == NULL || provider->structure_size < sizeof(*provider) ||
        provider->abi_version != 1U || provider->provider_id == NULL ||
        provider->provider_id[0] == '\0' || provider->generate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

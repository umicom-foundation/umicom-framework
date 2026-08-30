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

/* BEGINNER NOTE:
 * Validation prevents an incomplete or incompatible provider from being activated simply because a binary was discovered.
 */

#include "umicom/ai/provider.h"
#include <stddef.h>

UmiStatus umi_ai_provider_validate(const UmiAiProvider *provider)
{
    if (provider == NULL || provider->structure_size < sizeof(*provider) ||
        provider->abi_version != 1U || provider->provider_id == NULL ||
        provider->provider_id[0] == '\0' || provider->generate == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

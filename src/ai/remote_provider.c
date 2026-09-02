/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/remote_provider.c
 *
 * PURPOSE:
 *   Configure a remote AI provider endpoint without embedding provider-specific secrets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Remote provider configuration stores a secret reference rather than an API key, keeping credential resolution inside the secrets service.
 */

#include "umicom/ai/remote_provider.h"
#include <stddef.h>

/*
 * Check that ai remote provider satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_remote_provider_validate(const UmiAiRemoteProviderConfig *config)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (config == NULL || config->provider_id[0] == '\0' || config->endpoint[0] == '\0' || config->secret_reference[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

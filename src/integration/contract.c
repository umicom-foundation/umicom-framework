/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/contract.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/contract.h"

#include <string.h>

/*
 * Provide the integration contract compatible operation used by this module and its client
 * applications.
 */
bool umi_integration_contract_compatible(
    const UmiIntegrationContract *provider,
    const UmiIntegrationContract *consumer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (provider == NULL || consumer == NULL) {
        return false;
    }
    return strcmp(provider->id, consumer->id) == 0 &&
           provider->major == consumer->major &&
           provider->minor >= consumer->minor;
}

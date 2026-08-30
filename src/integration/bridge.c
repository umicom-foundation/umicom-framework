/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/bridge.c
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

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/bridge.h"

bool umi_integration_bridge_valid(const UmiIntegrationBridge *bridge)
{
    return bridge != NULL &&
           bridge->launch != NULL &&
           bridge->stop != NULL &&
           bridge->send != NULL;
}

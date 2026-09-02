/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/endpoint.c
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

#include "umicom/integration/endpoint.h"

/*
 * Check that integration endpoint satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_integration_endpoint_validate(
    const UmiIntegrationEndpoint *endpoint)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (endpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (endpoint->application_id[0] == '\0' ||
        endpoint->endpoint_id[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (endpoint->transport != UMI_INTEGRATION_TRANSPORT_IN_PROCESS &&
        endpoint->address[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

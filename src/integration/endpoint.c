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

UmiStatus umi_integration_endpoint_validate(
    const UmiIntegrationEndpoint *endpoint)
{
    if (endpoint == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (endpoint->application_id[0] == '\0' ||
        endpoint->endpoint_id[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    if (endpoint->transport != UMI_INTEGRATION_TRANSPORT_IN_PROCESS &&
        endpoint->address[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}

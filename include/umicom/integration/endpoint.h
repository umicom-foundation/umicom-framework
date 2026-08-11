/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/endpoint.h
 *
 * PURPOSE:
 *   Describe a public application endpoint without exposing product-private implementation details.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#ifndef UMICOM_INTEGRATION_ENDPOINT_H
#define UMICOM_INTEGRATION_ENDPOINT_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

typedef struct UmiIntegrationEndpoint {
    char application_id[UMI_INTEGRATION_ID_CAPACITY];
    char endpoint_id[UMI_INTEGRATION_ID_CAPACITY];
    UmiIntegrationTransport transport;
    char address[UMI_INTEGRATION_PATH_CAPACITY];
    bool local_only;
} UmiIntegrationEndpoint;

UmiStatus umi_integration_endpoint_validate(
    const UmiIntegrationEndpoint *endpoint);

#endif

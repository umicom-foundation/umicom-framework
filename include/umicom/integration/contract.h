/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/contract.h
 *
 * PURPOSE:
 *   Version a stable public inter-application contract independently from product internals.
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

#ifndef UMICOM_INTEGRATION_CONTRACT_H
#define UMICOM_INTEGRATION_CONTRACT_H

#include "umicom/integration/types.h"

typedef struct UmiIntegrationContract {
    char id[UMI_INTEGRATION_ID_CAPACITY];
    unsigned major;
    unsigned minor;
} UmiIntegrationContract;

bool umi_integration_contract_compatible(
    const UmiIntegrationContract *provider,
    const UmiIntegrationContract *consumer);

#endif

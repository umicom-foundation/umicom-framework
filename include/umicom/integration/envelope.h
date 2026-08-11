/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/envelope.h
 *
 * PURPOSE:
 *   Carry bounded command/event/query data across application boundaries.
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

#ifndef UMICOM_INTEGRATION_ENVELOPE_H
#define UMICOM_INTEGRATION_ENVELOPE_H

#include "umicom/base/status.h"
#include "umicom/integration/types.h"

typedef struct UmiIntegrationEnvelope {
    UmiIntegrationMessageKind kind;
    char source_application[UMI_INTEGRATION_ID_CAPACITY];
    char target_application[UMI_INTEGRATION_ID_CAPACITY];
    char topic[UMI_INTEGRATION_ID_CAPACITY];
    char correlation_id[UMI_INTEGRATION_ID_CAPACITY];
    char payload[UMI_INTEGRATION_PAYLOAD_CAPACITY];
} UmiIntegrationEnvelope;

void umi_integration_envelope_init(UmiIntegrationEnvelope *envelope);
UmiStatus umi_integration_envelope_validate(
    const UmiIntegrationEnvelope *envelope);

#endif

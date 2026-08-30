/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/connector_health.h
 *
 * PURPOSE:
 *   Aggregate connector connect/error evidence into readiness state and error rate.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_CONNECTOR_HEALTH_H
#define UMICOM_INTEGRATION_FABRIC_CONNECTOR_HEALTH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricConnectorHealth { uint64_t connects; uint64_t disconnects; uint64_t errors; uint64_t messages; UmiFabricState state; } UmiFabricConnectorHealth;
void umi_fabric_connector_health_init(UmiFabricConnectorHealth *health);
void umi_fabric_connector_health_record(UmiFabricConnectorHealth *health,bool connected,bool error,bool message);
double umi_fabric_connector_health_error_rate(const UmiFabricConnectorHealth *health);

#ifdef __cplusplus
}
#endif
#endif

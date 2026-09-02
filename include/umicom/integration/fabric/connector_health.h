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


/**
 * Represent the fabric connector health data shared with callers of this public contract.
 */
typedef struct UmiFabricConnectorHealth { uint64_t connects; uint64_t disconnects; uint64_t errors; uint64_t messages; UmiFabricState state; } UmiFabricConnectorHealth;
/**
 * Initialise fabric connector health from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_connector_health_init(UmiFabricConnectorHealth *health);
/**
 * Provide the fabric connector health record operation used by this module and its client
 * applications.
 */
void umi_fabric_connector_health_record(UmiFabricConnectorHealth *health,bool connected,bool error,bool message);
/**
 * Provide the fabric connector health error rate operation used by this module and its
 * client applications.
 */
double umi_fabric_connector_health_error_rate(const UmiFabricConnectorHealth *health);

#ifdef __cplusplus
}
#endif
#endif

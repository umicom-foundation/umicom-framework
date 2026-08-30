/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_health.h
 *
 * PURPOSE:
 *   Project service success/failure evidence into a stable health state without performing probes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_HEALTH_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_HEALTH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricServiceHealth { uint64_t successes; uint64_t failures; uint32_t consecutive_failures; uint64_t last_success_ms; uint64_t last_failure_ms; UmiFabricState state; } UmiFabricServiceHealth;
void umi_fabric_service_health_init(UmiFabricServiceHealth *health);
void umi_fabric_service_health_record(UmiFabricServiceHealth *health,bool success,uint64_t now_ms,uint32_t failure_threshold);

#ifdef __cplusplus
}
#endif
#endif

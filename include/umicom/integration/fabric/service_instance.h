/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_instance.h
 *
 * PURPOSE:
 *   Represent a live service instance advertised to the Fabric registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_INSTANCE_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_INSTANCE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric service instance data shared with callers of this public contract.
 */
typedef struct UmiFabricServiceInstance {
    char instance_id[UMI_FABRIC_ID_CAPACITY];
    char service_id[UMI_FABRIC_ID_CAPACITY];
    char endpoint_id[UMI_FABRIC_ID_CAPACITY];
    uint32_t priority;
    uint32_t weight;
    bool healthy;
    uint64_t last_seen_ms;
} UmiFabricServiceInstance;

/**
 * Initialise fabric service instance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_service_instance_init(UmiFabricServiceInstance *item, const char *instance_id, const char *service_id, const char *endpoint_id, uint32_t priority, uint32_t weight);
/**
 * Check that fabric service instance satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_service_instance_validate(const UmiFabricServiceInstance *item);

#ifdef __cplusplus
}
#endif
#endif

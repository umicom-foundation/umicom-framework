/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_registry.h
 *
 * PURPOSE:
 *   Track unique live service instances separately from immutable service contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_REGISTRY_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_REGISTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/service_instance.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric service registry data shared with callers of this public contract.
 */
typedef struct UmiFabricServiceRegistry {
    UmiFabricServiceInstance items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricServiceRegistry;
/**
 * Initialise fabric service registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_service_registry_init(UmiFabricServiceRegistry *registry);
/**
 * Add fabric service registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_service_registry_add(UmiFabricServiceRegistry *registry,const UmiFabricServiceInstance *item);
/**
 * Find fabric service registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_fabric_service_registry_find(const UmiFabricServiceRegistry *registry,const char *id,UmiFabricServiceInstance *out_item);
/**
 * Return the number of records represented by fabric service registry without changing
 * their state.
 */
size_t umi_fabric_service_registry_count(const UmiFabricServiceRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

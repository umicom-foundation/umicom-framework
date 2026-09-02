/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/endpoint_registry.h
 *
 * PURPOSE:
 *   Maintain unique endpoint definitions used by service instances and connectors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_ENDPOINT_REGISTRY_H
#define UMICOM_INTEGRATION_FABRIC_ENDPOINT_REGISTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/endpoint_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric endpoint registry data shared with callers of this public contract.
 */
typedef struct UmiFabricEndpointRegistry {
    UmiFabricEndpointDescriptor items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricEndpointRegistry;
/**
 * Initialise fabric endpoint registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_endpoint_registry_init(UmiFabricEndpointRegistry *registry);
/**
 * Add fabric endpoint registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_endpoint_registry_add(UmiFabricEndpointRegistry *registry,const UmiFabricEndpointDescriptor *item);
/**
 * Find fabric endpoint registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_fabric_endpoint_registry_find(const UmiFabricEndpointRegistry *registry,const char *id,UmiFabricEndpointDescriptor *out_item);
/**
 * Return the number of records represented by fabric endpoint registry without changing
 * their state.
 */
size_t umi_fabric_endpoint_registry_count(const UmiFabricEndpointRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

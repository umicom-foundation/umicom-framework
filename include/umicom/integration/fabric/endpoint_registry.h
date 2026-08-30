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

typedef struct UmiFabricEndpointRegistry {
    UmiFabricEndpointDescriptor items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricEndpointRegistry;
void umi_fabric_endpoint_registry_init(UmiFabricEndpointRegistry *registry);
UmiStatus umi_fabric_endpoint_registry_add(UmiFabricEndpointRegistry *registry,const UmiFabricEndpointDescriptor *item);
UmiStatus umi_fabric_endpoint_registry_find(const UmiFabricEndpointRegistry *registry,const char *id,UmiFabricEndpointDescriptor *out_item);
size_t umi_fabric_endpoint_registry_count(const UmiFabricEndpointRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

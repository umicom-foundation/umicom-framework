/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_registry.h
 *
 * PURPOSE:
 *   Track unique live service instances separately from immutable service contracts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFabricServiceRegistry {
    UmiFabricServiceInstance items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricServiceRegistry;
void umi_fabric_service_registry_init(UmiFabricServiceRegistry *registry);
UmiStatus umi_fabric_service_registry_add(UmiFabricServiceRegistry *registry,const UmiFabricServiceInstance *item);
UmiStatus umi_fabric_service_registry_find(const UmiFabricServiceRegistry *registry,const char *id,UmiFabricServiceInstance *out_item);
size_t umi_fabric_service_registry_count(const UmiFabricServiceRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

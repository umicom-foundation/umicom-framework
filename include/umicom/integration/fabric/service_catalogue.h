/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_catalogue.h
 *
 * PURPOSE:
 *   Maintain unique service contracts for discovery and compatibility negotiation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_CATALOGUE_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/service_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricServiceCatalogue {
    UmiFabricServiceDescriptor items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricServiceCatalogue;
void umi_fabric_service_catalogue_init(UmiFabricServiceCatalogue *registry);
UmiStatus umi_fabric_service_catalogue_add(UmiFabricServiceCatalogue *registry,const UmiFabricServiceDescriptor *item);
UmiStatus umi_fabric_service_catalogue_find(const UmiFabricServiceCatalogue *registry,const char *id,UmiFabricServiceDescriptor *out_item);
size_t umi_fabric_service_catalogue_count(const UmiFabricServiceCatalogue *registry);

#ifdef __cplusplus
}
#endif
#endif

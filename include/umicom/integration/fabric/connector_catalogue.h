/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/connector_catalogue.h
 *
 * PURPOSE:
 *   Maintain unique connector descriptors so applications consume Framework-owned adapters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_CONNECTOR_CATALOGUE_H
#define UMICOM_INTEGRATION_FABRIC_CONNECTOR_CATALOGUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/connector_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricConnectorCatalogue {
    UmiFabricConnectorDescriptor items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricConnectorCatalogue;
void umi_fabric_connector_catalogue_init(UmiFabricConnectorCatalogue *registry);
UmiStatus umi_fabric_connector_catalogue_add(UmiFabricConnectorCatalogue *registry,const UmiFabricConnectorDescriptor *item);
UmiStatus umi_fabric_connector_catalogue_find(const UmiFabricConnectorCatalogue *registry,const char *id,UmiFabricConnectorDescriptor *out_item);
size_t umi_fabric_connector_catalogue_count(const UmiFabricConnectorCatalogue *registry);

#ifdef __cplusplus
}
#endif
#endif

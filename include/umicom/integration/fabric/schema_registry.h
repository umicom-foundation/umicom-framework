/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/schema_registry.h
 *
 * PURPOSE:
 *   Maintain unique schema versions using a caller-defined stable schema-version identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SCHEMA_REGISTRY_H
#define UMICOM_INTEGRATION_FABRIC_SCHEMA_REGISTRY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/schema_descriptor.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricSchemaRegistry {
    UmiFabricSchemaDescriptor items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricSchemaRegistry;
void umi_fabric_schema_registry_init(UmiFabricSchemaRegistry *registry);
UmiStatus umi_fabric_schema_registry_add(UmiFabricSchemaRegistry *registry,const UmiFabricSchemaDescriptor *item);
UmiStatus umi_fabric_schema_registry_find(const UmiFabricSchemaRegistry *registry,const char *id,UmiFabricSchemaDescriptor *out_item);
size_t umi_fabric_schema_registry_count(const UmiFabricSchemaRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

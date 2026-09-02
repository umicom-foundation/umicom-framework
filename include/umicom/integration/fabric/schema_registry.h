/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/schema_registry.h
 *
 * PURPOSE:
 *   Maintain unique schema versions using a caller-defined stable schema-version identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the fabric schema registry data shared with callers of this public contract.
 */
typedef struct UmiFabricSchemaRegistry {
    UmiFabricSchemaDescriptor items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricSchemaRegistry;
/**
 * Initialise fabric schema registry from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_schema_registry_init(UmiFabricSchemaRegistry *registry);
/**
 * Add fabric schema registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_schema_registry_add(UmiFabricSchemaRegistry *registry,const UmiFabricSchemaDescriptor *item);
/**
 * Find fabric schema registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_fabric_schema_registry_find(const UmiFabricSchemaRegistry *registry,const char *id,UmiFabricSchemaDescriptor *out_item);
/**
 * Return the number of records represented by fabric schema registry without changing
 * their state.
 */
size_t umi_fabric_schema_registry_count(const UmiFabricSchemaRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif

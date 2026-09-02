/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_catalogue.h
 *
 * PURPOSE:
 *   Maintain unique service contracts for discovery and compatibility negotiation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the fabric service catalogue data shared with callers of this public contract.
 */
typedef struct UmiFabricServiceCatalogue {
    UmiFabricServiceDescriptor items[UMI_FABRIC_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiFabricServiceCatalogue;
/**
 * Initialise fabric service catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_fabric_service_catalogue_init(UmiFabricServiceCatalogue *registry);
/**
 * Add fabric service catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_fabric_service_catalogue_add(UmiFabricServiceCatalogue *registry,const UmiFabricServiceDescriptor *item);
/**
 * Find fabric service catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_fabric_service_catalogue_find(const UmiFabricServiceCatalogue *registry,const char *id,UmiFabricServiceDescriptor *out_item);
/**
 * Return the number of records represented by fabric service catalogue without changing
 * their state.
 */
size_t umi_fabric_service_catalogue_count(const UmiFabricServiceCatalogue *registry);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/service_descriptor.h
 *
 * PURPOSE:
 *   Describe a reusable integration service, version and advertised capability mask.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SERVICE_DESCRIPTOR_H
#define UMICOM_INTEGRATION_FABRIC_SERVICE_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric service descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricServiceDescriptor {
    char service_id[UMI_FABRIC_ID_CAPACITY];
    char name[UMI_FABRIC_TEXT_CAPACITY];
    UmiFabricVersion version;
    uint64_t capability_mask;
} UmiFabricServiceDescriptor;

/**
 * Initialise fabric service descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_service_descriptor_init(UmiFabricServiceDescriptor *item, const char *service_id, const char *name, UmiFabricVersion version, uint64_t capability_mask);
/**
 * Check that fabric service descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_service_descriptor_validate(const UmiFabricServiceDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/schema_descriptor.h
 *
 * PURPOSE:
 *   Describe a versioned integration schema and compatibility intent independently of serialization format.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_SCHEMA_DESCRIPTOR_H
#define UMICOM_INTEGRATION_FABRIC_SCHEMA_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric schema descriptor data shared with callers of this public contract.
 */
typedef struct UmiFabricSchemaDescriptor {
    char schema_id[UMI_FABRIC_ID_CAPACITY];
    char name[UMI_FABRIC_TEXT_CAPACITY];
    UmiFabricVersion version;
    uint64_t fingerprint;
    bool backward_compatible;
    bool forward_compatible;
} UmiFabricSchemaDescriptor;

/**
 * Initialise fabric schema descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_schema_descriptor_init(UmiFabricSchemaDescriptor *item, const char *schema_id, const char *name, UmiFabricVersion version, uint64_t fingerprint, bool backward_compatible, bool forward_compatible);
/**
 * Check that fabric schema descriptor satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_schema_descriptor_validate(const UmiFabricSchemaDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif

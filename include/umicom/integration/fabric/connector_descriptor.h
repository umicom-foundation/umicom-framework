/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/connector_descriptor.h
 *
 * PURPOSE:
 *   Describe a connector implementation and supported protocol/capability surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_CONNECTOR_DESCRIPTOR_H
#define UMICOM_INTEGRATION_FABRIC_CONNECTOR_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric connector descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiFabricConnectorDescriptor {
    char connector_id[UMI_FABRIC_ID_CAPACITY];
    char provider[UMI_FABRIC_TEXT_CAPACITY];
    UmiFabricProtocol protocol;
    uint64_t capability_mask;
    bool supports_transactions;
} UmiFabricConnectorDescriptor;

/**
 * Initialise fabric connector descriptor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_connector_descriptor_init(UmiFabricConnectorDescriptor *item, const char *connector_id, const char *provider, UmiFabricProtocol protocol, uint64_t capability_mask, bool supports_transactions);
/**
 * Check that fabric connector descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_fabric_connector_descriptor_validate(const UmiFabricConnectorDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif

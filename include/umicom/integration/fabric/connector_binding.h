/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/connector_binding.h
 *
 * PURPOSE:
 *   Bind a connector to one endpoint/profile without embedding product-specific connection logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_CONNECTOR_BINDING_H
#define UMICOM_INTEGRATION_FABRIC_CONNECTOR_BINDING_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric connector binding data shared with callers of this public contract.
 */
typedef struct UmiFabricConnectorBinding {
    char binding_id[UMI_FABRIC_ID_CAPACITY];
    char connector_id[UMI_FABRIC_ID_CAPACITY];
    char endpoint_id[UMI_FABRIC_ID_CAPACITY];
    char transport_profile_id[UMI_FABRIC_ID_CAPACITY];
    bool enabled;
} UmiFabricConnectorBinding;

/**
 * Initialise fabric connector binding from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_fabric_connector_binding_init(UmiFabricConnectorBinding *item, const char *binding_id, const char *connector_id, const char *endpoint_id, const char *transport_profile_id);
/**
 * Check that fabric connector binding satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_fabric_connector_binding_validate(const UmiFabricConnectorBinding *item);

#ifdef __cplusplus
}
#endif
#endif

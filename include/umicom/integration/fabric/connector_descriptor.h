/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/connector_descriptor.h
 *
 * PURPOSE:
 *   Describe a connector implementation and supported protocol/capability surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiFabricConnectorDescriptor {
    char connector_id[UMI_FABRIC_ID_CAPACITY];
    char provider[UMI_FABRIC_TEXT_CAPACITY];
    UmiFabricProtocol protocol;
    uint64_t capability_mask;
    bool supports_transactions;
} UmiFabricConnectorDescriptor;

UmiStatus umi_fabric_connector_descriptor_init(UmiFabricConnectorDescriptor *item, const char *connector_id, const char *provider, UmiFabricProtocol protocol, uint64_t capability_mask, bool supports_transactions);
UmiStatus umi_fabric_connector_descriptor_validate(const UmiFabricConnectorDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/endpoint_descriptor.h
 *
 * PURPOSE:
 *   Describe a protocol endpoint independently from any transport implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_ENDPOINT_DESCRIPTOR_H
#define UMICOM_INTEGRATION_FABRIC_ENDPOINT_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricEndpointDescriptor {
    char endpoint_id[UMI_FABRIC_ID_CAPACITY];
    char uri[UMI_FABRIC_URI_CAPACITY];
    UmiFabricProtocol protocol;
    bool secure;
    uint32_t weight;
} UmiFabricEndpointDescriptor;

UmiStatus umi_fabric_endpoint_descriptor_init(UmiFabricEndpointDescriptor *item, const char *endpoint_id, const char *uri, UmiFabricProtocol protocol, bool secure, uint32_t weight);
UmiStatus umi_fabric_endpoint_descriptor_validate(const UmiFabricEndpointDescriptor *item);

#ifdef __cplusplus
}
#endif
#endif

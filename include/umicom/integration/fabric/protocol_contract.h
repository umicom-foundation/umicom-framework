/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/protocol_contract.h
 *
 * PURPOSE:
 *   Describe versioned protocol expectations and streaming semantics for endpoint negotiation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_PROTOCOL_CONTRACT_H
#define UMICOM_INTEGRATION_FABRIC_PROTOCOL_CONTRACT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiFabricProtocolContract {
    char contract_id[UMI_FABRIC_ID_CAPACITY];
    UmiFabricProtocol protocol;
    UmiFabricVersion minimum_version;
    UmiFabricVersion maximum_version;
    bool streaming;
    bool bidirectional;
} UmiFabricProtocolContract;

UmiStatus umi_fabric_protocol_contract_init(UmiFabricProtocolContract *item, const char *contract_id, UmiFabricProtocol protocol, UmiFabricVersion minimum_version, UmiFabricVersion maximum_version, bool streaming, bool bidirectional);
UmiStatus umi_fabric_protocol_contract_validate(const UmiFabricProtocolContract *item);

#ifdef __cplusplus
}
#endif
#endif

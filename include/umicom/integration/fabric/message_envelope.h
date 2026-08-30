/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/message_envelope.h
 *
 * PURPOSE:
 *   Carry bounded payload evidence separately from the canonical messaging implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_MESSAGE_ENVELOPE_H
#define UMICOM_INTEGRATION_FABRIC_MESSAGE_ENVELOPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/message_header.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricMessageEnvelope { UmiFabricMessageHeader header; uint64_t payload_length; uint64_t payload_hash; bool compressed; } UmiFabricMessageEnvelope;
UmiStatus umi_fabric_message_envelope_init(UmiFabricMessageEnvelope *envelope,const UmiFabricMessageHeader *header,const void *payload,size_t payload_length,bool compressed);
bool umi_fabric_message_envelope_matches(const UmiFabricMessageEnvelope *envelope,const void *payload,size_t payload_length);

#ifdef __cplusplus
}
#endif
#endif

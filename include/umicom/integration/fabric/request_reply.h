/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/request_reply.h
 *
 * PURPOSE:
 *   Model request/reply state and reject responses with mismatched correlation identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_REQUEST_REPLY_H
#define UMICOM_INTEGRATION_FABRIC_REQUEST_REPLY_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/message_header.h"
#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the fabric request reply data shared with callers of this public contract.
 */
typedef struct UmiFabricRequestReply { char request_id[UMI_FABRIC_ID_CAPACITY]; char correlation_id[UMI_FABRIC_ID_CAPACITY]; bool completed; int32_t response_code; uint64_t completed_ms; } UmiFabricRequestReply;
/**
 * Provide the fabric request reply begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_request_reply_begin(UmiFabricRequestReply *state,const UmiFabricMessageHeader *request);
/**
 * Provide the fabric request reply complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_request_reply_complete(UmiFabricRequestReply *state,const UmiFabricMessageHeader *reply,int32_t response_code,uint64_t now_ms);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/fabric/request_reply.c
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
#include "umicom/integration/fabric/request_reply.h"
#include <string.h>
#include <limits.h>


/*
 * Provide the fabric request reply begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_request_reply_begin(UmiFabricRequestReply *state,const UmiFabricMessageHeader *request) { UmiStatus s; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(state==NULL||request==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(state,0,sizeof(*state));s=umi_fabric_copy_text(state->request_id,sizeof(state->request_id),request->message_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_fabric_copy_text(state->correlation_id,sizeof(state->correlation_id),request->correlation_id); }
/*
 * Provide the fabric request reply complete operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_request_reply_complete(UmiFabricRequestReply *state,const UmiFabricMessageHeader *reply,int32_t response_code,uint64_t now_ms) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if(state==NULL||reply==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(state->completed)return UMI_STATUS_INVALID_STATE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(state->correlation_id,reply->correlation_id)!=0)return UMI_STATUS_INVALID_STATE;state->completed=true;state->response_code=response_code;state->completed_ms=now_ms;return UMI_STATUS_OK; }

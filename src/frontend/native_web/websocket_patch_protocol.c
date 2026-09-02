/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/websocket_patch_protocol.c
 *
 * PURPOSE:
 *   Frame serialized DOM patch batches for delivery through the existing Framework WebSocket transport.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/websocket_patch_protocol.h"

/*
 * Write native web websocket patch protocol in its stable representation and report
 * capacity or input failures to the caller.
 */
UmiStatus umi_native_web_websocket_patch_protocol_encode(const UmiNativeWebPatchBatch *batch,UmiNativeWebMarkupBuffer *out_payload){UmiNativeWebMarkupBuffer json;UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(batch==NULL||out_payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_native_web_patch_serialize(batch,&json);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;umi_native_web_markup_buffer_init(out_payload);s=umi_native_web_markup_buffer_append(out_payload,"PATCH ");/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_payload,json.data);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;return umi_native_web_markup_buffer_append(out_payload,"");}


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

UmiStatus umi_native_web_websocket_patch_protocol_encode(const UmiNativeWebPatchBatch *batch,UmiNativeWebMarkupBuffer *out_payload){UmiNativeWebMarkupBuffer json;UmiStatus s;if(batch==NULL||out_payload==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_native_web_patch_serialize(batch,&json);if(s!=UMI_STATUS_OK)return s;umi_native_web_markup_buffer_init(out_payload);s=umi_native_web_markup_buffer_append(out_payload,"PATCH ");if(s!=UMI_STATUS_OK)return s;s=umi_native_web_markup_buffer_append(out_payload,json.data);if(s!=UMI_STATUS_OK)return s;return umi_native_web_markup_buffer_append(out_payload,"");}


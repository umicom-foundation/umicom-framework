/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/sse_patch_protocol.h
 *
 * PURPOSE:
 *   Frame serialized DOM patch batches as Server-Sent Events over the existing Framework web service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_SSE_PATCH_PROTOCOL_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_SSE_PATCH_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/patch_serializer.h"
/* Encode one patch batch for sse delivery over the existing Framework web transport. */
UmiStatus umi_native_web_sse_patch_protocol_encode(const UmiNativeWebPatchBatch *batch, UmiNativeWebMarkupBuffer *out_payload);

#ifdef __cplusplus
}
#endif
#endif

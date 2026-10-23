/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/patch_serializer.h
 *
 * PURPOSE:
 *   Serialize patch batches into the compact native-web JSON patch protocol.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_SERIALIZER_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_PATCH_SERIALIZER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/patch_batch.h"
#include "umicom/frontend/native_web/markup_buffer.h"
/* Serialize a patch batch as compact JSON understood by the generated browser runtime. */
/** Bounded batch and inline strings are checked before publishing output.
 * Control characters use JSON escapes; malformed UTF-8 is refused. Failure
 * leaves the destination unchanged. Inputs must not overlap the destination. */
UmiStatus umi_native_web_patch_serialize(const UmiNativeWebPatchBatch *batch, UmiNativeWebMarkupBuffer *out_json);

#ifdef __cplusplus
}
#endif
#endif

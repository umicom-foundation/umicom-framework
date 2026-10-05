/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/render_context.h
 *
 * PURPOSE:
 *   Carry session, route, theme, density, locale and revision state through web rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_CONTEXT_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_CONTEXT_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"

/**
 * Represent the native web render context data shared with callers of this public
 * contract.
 */
typedef struct UmiNativeWebRenderContext { char session_id[UMI_NATIVE_WEB_ID_CAPACITY]; char route[UMI_NATIVE_WEB_TEXT_CAPACITY]; char theme[64]; char density[64]; char locale[32]; uint64_t revision; } UmiNativeWebRenderContext;
/* Initialise render context with stable session and route identity. */
UmiStatus umi_native_web_render_context_init(UmiNativeWebRenderContext *context, const char *session_id, const char *route);
/* Validate required render context identity. */
UmiStatus umi_native_web_render_context_validate(const UmiNativeWebRenderContext *context);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_native_web_render_context_archive_encode(const UmiNativeWebRenderContext *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_native_web_render_context_archive_decode(const void *bytes, size_t byte_count,
    UmiNativeWebRenderContext *value);

#ifdef __cplusplus
}
#endif
#endif

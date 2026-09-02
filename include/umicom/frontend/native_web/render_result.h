/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/render_result.h
 *
 * PURPOSE:
 *   Accumulate HTML, CSS and patch output plus render revision/evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_RESULT_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_RENDER_RESULT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/markup_buffer.h"
#include "umicom/frontend/native_web/css_buffer.h"

/**
 * Represent the native web render result data shared with callers of this public contract.
 */
typedef struct UmiNativeWebRenderResult { UmiNativeWebMarkupBuffer markup; UmiNativeWebCssBuffer css; uint64_t revision; size_t rendered_elements; } UmiNativeWebRenderResult;
/* Initialise empty HTML/CSS output for one render operation. */
void umi_native_web_render_result_init(UmiNativeWebRenderResult *result, uint64_t revision);
/* Mark one semantic element as emitted. */
void umi_native_web_render_result_note_element(UmiNativeWebRenderResult *result);

#ifdef __cplusplus
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/document_shell.h
 *
 * PURPOSE:
 *   Generate a complete accessible HTML document shell for a server-driven Umicom application.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_DOCUMENT_SHELL_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_DOCUMENT_SHELL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/render_context.h"
#include "umicom/frontend/native_web/markup_buffer.h"
/* Build a standards-based HTML document shell around already-rendered workbench markup. */
UmiStatus umi_native_web_document_shell_build(const UmiNativeWebRenderContext *context, const char *title, const char *body_markup, const char *css, const char *runtime_js, UmiNativeWebMarkupBuffer *out_document);

#ifdef __cplusplus
}
#endif
#endif

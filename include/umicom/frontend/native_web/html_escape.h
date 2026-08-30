/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/html_escape.h
 *
 * PURPOSE:
 *   Escape HTML text and attribute values without third-party templating dependencies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_HTML_ESCAPE_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_HTML_ESCAPE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/markup_buffer.h"
/* Escape text-node content into the supplied bounded buffer. */
UmiStatus umi_native_web_html_escape_text(UmiNativeWebMarkupBuffer *buffer, const char *text);
/* Escape quoted HTML attribute content into the supplied bounded buffer. */
UmiStatus umi_native_web_html_escape_attribute(UmiNativeWebMarkupBuffer *buffer, const char *text);

#ifdef __cplusplus
}
#endif
#endif

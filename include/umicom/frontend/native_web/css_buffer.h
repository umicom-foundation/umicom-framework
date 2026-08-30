/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/css_buffer.h
 *
 * PURPOSE:
 *   Build bounded semantic CSS rules and declarations from Framework theme tokens.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_CSS_BUFFER_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_CSS_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"

typedef struct UmiNativeWebCssBuffer { char data[UMI_NATIVE_WEB_CSS_CAPACITY]; size_t length; UmiStatus status; } UmiNativeWebCssBuffer;
/* Initialise an empty bounded CSS buffer. */
void umi_native_web_css_buffer_init(UmiNativeWebCssBuffer *buffer);
/* Append one validated selector/property/value declaration. */
UmiStatus umi_native_web_css_buffer_rule(UmiNativeWebCssBuffer *buffer, const char *selector, const char *property, const char *value);

#ifdef __cplusplus
}
#endif
#endif

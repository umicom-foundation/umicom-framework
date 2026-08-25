/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/css_buffer.c
 *
 * PURPOSE:
 *   Build bounded semantic CSS rules and declarations from Framework theme tokens.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/css_buffer.h"

#include <stdio.h>
#include <string.h>

void umi_native_web_css_buffer_init(UmiNativeWebCssBuffer *buffer)
{
    if (buffer == NULL) return;
    buffer->data[0] = '\0'; buffer->length = 0U; buffer->status = UMI_STATUS_OK;
}

UmiStatus umi_native_web_css_buffer_rule(UmiNativeWebCssBuffer *buffer, const char *selector, const char *property, const char *value)
{
    int written;
    if (buffer == NULL || selector == NULL || property == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strchr(selector, '{') != NULL || strchr(property, ':') != NULL || strchr(value, '}') != NULL) return UMI_STATUS_PARSE_ERROR;
    if (buffer->length >= UMI_NATIVE_WEB_CSS_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(buffer->data + buffer->length, UMI_NATIVE_WEB_CSS_CAPACITY - buffer->length, "%s{%s:%s;}\n", selector, property, value);
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    if ((size_t)written >= UMI_NATIVE_WEB_CSS_CAPACITY - buffer->length) { buffer->status = UMI_STATUS_CAPACITY_EXCEEDED; return buffer->status; }
    buffer->length += (size_t)written;
    return UMI_STATUS_OK;
}


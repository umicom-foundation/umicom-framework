/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/css_buffer.c
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

#include "umicom/frontend/native_web/css_buffer.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise native web css buffer from caller-provided values so later operations receive
 * a known state.
 */
void umi_native_web_css_buffer_init(UmiNativeWebCssBuffer *buffer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return;
    buffer->data[0] = '\0'; buffer->length = 0U; buffer->status = UMI_STATUS_OK;
}

/*
 * Provide the native web css buffer rule operation used by this module and its client
 * applications.
 */
UmiStatus umi_native_web_css_buffer_rule(UmiNativeWebCssBuffer *buffer, const char *selector, const char *property, const char *value)
{
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || selector == NULL || property == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strchr(selector, '{') != NULL || strchr(property, ':') != NULL || strchr(value, '}') != NULL) return UMI_STATUS_PARSE_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (buffer->length >= UMI_NATIVE_WEB_CSS_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    written = snprintf(buffer->data + buffer->length, UMI_NATIVE_WEB_CSS_CAPACITY - buffer->length, "%s{%s:%s;}\n", selector, property, value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (written < 0) return UMI_STATUS_INTERNAL_ERROR;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)written >= UMI_NATIVE_WEB_CSS_CAPACITY - buffer->length) { buffer->status = UMI_STATUS_CAPACITY_EXCEEDED; return buffer->status; }
    buffer->length += (size_t)written;
    return UMI_STATUS_OK;
}


/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/native_web/markup_buffer.c
 *
 * PURPOSE:
 *   Provide bounded C23 markup construction with explicit truncation/error state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/frontend/native_web/markup_buffer.h"

#include <string.h>

void umi_native_web_markup_buffer_init(UmiNativeWebMarkupBuffer *buffer)
{
    if (buffer == NULL) return;
    buffer->data[0] = '\0';
    buffer->length = 0U;
    buffer->status = UMI_STATUS_OK;
}

UmiStatus umi_native_web_markup_buffer_append_n(UmiNativeWebMarkupBuffer *buffer, const char *text, size_t length)
{
    if (buffer == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (buffer->status != UMI_STATUS_OK) return buffer->status;
    if (length > (UMI_NATIVE_WEB_MARKUP_CAPACITY - 1U) - buffer->length) {
        buffer->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return buffer->status;
    }
    (void)memcpy(buffer->data + buffer->length, text, length);
    buffer->length += length;
    buffer->data[buffer->length] = '\0';
    return UMI_STATUS_OK;
}

UmiStatus umi_native_web_markup_buffer_append(UmiNativeWebMarkupBuffer *buffer, const char *text)
{
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_native_web_markup_buffer_append_n(buffer, text, strlen(text));
}


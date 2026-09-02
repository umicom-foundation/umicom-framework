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

/*
 * Initialise native web markup buffer from caller-provided values so later operations
 * receive a known state.
 */
void umi_native_web_markup_buffer_init(UmiNativeWebMarkupBuffer *buffer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL) return;
    buffer->data[0] = '\0';
    buffer->length = 0U;
    buffer->status = UMI_STATUS_OK;
}

/*
 * Provide the native web markup buffer append n operation used by this module and its
 * client applications.
 */
UmiStatus umi_native_web_markup_buffer_append_n(UmiNativeWebMarkupBuffer *buffer, const char *text, size_t length)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (buffer == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (buffer->status != UMI_STATUS_OK) return buffer->status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > (UMI_NATIVE_WEB_MARKUP_CAPACITY - 1U) - buffer->length) {
        buffer->status = UMI_STATUS_CAPACITY_EXCEEDED;
        return buffer->status;
    }
    (void)memcpy(buffer->data + buffer->length, text, length);
    buffer->length += length;
    buffer->data[buffer->length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Add native web markup buffer only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_native_web_markup_buffer_append(UmiNativeWebMarkupBuffer *buffer, const char *text)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_native_web_markup_buffer_append_n(buffer, text, strlen(text));
}


/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/memory_capture.c
 * PURPOSE: Render owned bytes without interpreting target pointer widths or byte order.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "memory_inspection_private.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void UmiDebugMemoryCaptureDestroy(UmiDebugMemoryCapture *capture) { free(capture); }
UmiStatus UmiDebugMemoryCaptureRead(const UmiDebugMemoryCapture *capture, UmiDebugMemoryBytes *out)
{
    if (capture == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = capture->result;
    return UMI_STATUS_OK;
}
UmiStatus UmiDebugMemoryCaptureValidate(const UmiDebugMemoryCapture *capture, UmiDebugWorkspace *workspace)
{
    if (capture == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiDebugVariableTargetValidate(workspace, &capture->target);
}
UmiStatus UmiDebugMemoryCaptureFormat(const UmiDebugMemoryCapture *capture, char *out, size_t capacity,
                                      size_t *required)
{
    if (capture == NULL || required == NULL || (out == NULL && capacity != 0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* 4096 bytes need 256 rows of fewer than 80 characters plus the summary.
     * Build off-output so undersized callers never receive a plausible prefix. */
    const size_t limit = 32768U;
    char *text = malloc(limit);
    if (text == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    const UmiDebugMemoryBytes *value = &capture->result;
    int written =
        snprintf(text, limit,
                 "Captured memory; values do not update automatically.\nAddress: %s\nRequested offset: %lld "
                 "bytes; requested: %u bytes\nReturned: %zu bytes; unreadable after data: %llu "
                 "bytes\n%s\nRelative  Hex bytes                                         ASCII\n",
                 value->address, (long long)capture->offset, (unsigned)capture->requested, value->count,
                 (unsigned long long)value->unreadable,
                 value->count < capture->requested && value->unreadable == 0U
                     ? "The adapter reached the end of readable memory."
                     : "Row offsets below start at the returned address.");
    UmiStatus status = UMI_STATUS_OK;
    size_t used = written > 0 ? (size_t)written : limit;
    if (used >= limit)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t start = 0U; status == UMI_STATUS_OK && start < value->count; start += 16U)
    {
        char hex[49], ascii[17];
        memset(hex, ' ', 48U);
        hex[48] = '\0';
        size_t count = value->count - start;
        if (count > 16U)
            count = 16U;
        static const char digits[] = "0123456789ABCDEF";
        for (size_t i = 0U; i < count; ++i)
        {
            unsigned char byte = value->bytes[start + i];
            hex[i * 3U] = digits[byte >> 4];
            hex[i * 3U + 1U] = digits[byte & 15U];
            ascii[i] = byte >= 32U && byte <= 126U ? (char)byte : '.';
        }
        ascii[count] = '\0';
        written = snprintf(text + used, limit - used, "+%04zX    %s  |%s|\n", start, hex, ascii);
        if (written < 0 || (size_t)written >= limit - used)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            used += (size_t)written;
    }
    if (status == UMI_STATUS_OK)
    {
        *required = used + 1U;
        if (capacity < *required)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        else
            memcpy(out, text, *required);
    }
    free(text);
    return status;
}

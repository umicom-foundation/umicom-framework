/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/native_web/markup_buffer.h
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

#ifndef UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_MARKUP_BUFFER_H
#define UMICOM_INCLUDE_UMICOM_FRONTEND_NATIVE_WEB_MARKUP_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/frontend/native_web/types.h"

typedef struct UmiNativeWebMarkupBuffer {
    char data[UMI_NATIVE_WEB_MARKUP_CAPACITY];
    size_t length;
    UmiStatus status;
} UmiNativeWebMarkupBuffer;

/* Initialise an empty bounded markup buffer. */
void umi_native_web_markup_buffer_init(UmiNativeWebMarkupBuffer *buffer);
/* Append raw bytes after checking capacity. */
UmiStatus umi_native_web_markup_buffer_append(UmiNativeWebMarkupBuffer *buffer, const char *text);
/* Append a bounded byte span after checking capacity. */
UmiStatus umi_native_web_markup_buffer_append_n(UmiNativeWebMarkupBuffer *buffer, const char *text, size_t length);

#ifdef __cplusplus
}
#endif
#endif

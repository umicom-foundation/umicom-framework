/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/sse.h
 *
 * PURPOSE:
 *   Format Server-Sent Events for browser streaming.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module has one narrow responsibility. Keeping the pieces separate makes the web platform easier to test and lets Studio, Trader and TMS reuse the same implementation.
 */

#ifndef UMICOM_WEB_SSE_H
#define UMICOM_WEB_SSE_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiWebSseEvent { uint64_t id; char event[64]; char data[2048]; } UmiWebSseEvent;
UmiStatus umi_web_sse_format(const UmiWebSseEvent *event,char *out_text,size_t capacity,size_t *out_length);
#ifdef __cplusplus
}
#endif
#endif

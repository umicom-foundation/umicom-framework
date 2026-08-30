/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/sse.c
 *
 * PURPOSE:
 *   Implement Server-Sent Event formatting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/sse.h"
#include <stdio.h>
UmiStatus umi_web_sse_format(const UmiWebSseEvent *event,char *out_text,size_t capacity,size_t *out_length){int n;if(event==NULL||out_text==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out_text,capacity,"id: %llu\nevent: %s\ndata: %s\n\n",(unsigned long long)event->id,event->event,event->data);if(n<0||(size_t)n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;if(out_length!=NULL)*out_length=(size_t)n;return UMI_STATUS_OK;}

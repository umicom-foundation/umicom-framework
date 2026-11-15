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

/*
 * The code below implements one small part of the web stack. It uses bounded data and explicit status values so failures are visible and testable.
 */

#include "umicom/web/sse.h"
#include <stdio.h>
#include <string.h>
#include "umicom/base/utf8.h"
/* Provide the web sse format operation used by this module and its client applications. */
/* Validate fixed input fields and stage one complete record before publication. Multiline data now stays inside data fields and event-name delimiters are refused. The former direct snprintf implementation remains for review; ordinary single-line output is preserved. The previous implementation remains for review. */
#if 0
UmiStatus umi_web_sse_format(const UmiWebSseEvent *event,char *out_text,size_t capacity,size_t *out_length){int n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(event==NULL||out_text==NULL)return UMI_STATUS_INVALID_ARGUMENT;n=snprintf(out_text,capacity,"id: %llu\nevent: %s\ndata: %s\n\n",(unsigned long long)event->id,event->event,event->data);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n<0||(size_t)n>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_length!=NULL)*out_length=(size_t)n;return UMI_STATUS_OK;}
#endif
UmiStatus umi_web_sse_format(const UmiWebSseEvent *event,char *out_text,
    size_t capacity,size_t *out_length)
{
    if(event==NULL || out_text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    const char *event_end=memchr(event->event,'\0',sizeof event->event);
    const char *data_end=memchr(event->data,'\0',sizeof event->data);
    if(event_end==NULL || data_end==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    size_t event_length=(size_t)(event_end-event->event);
    size_t data_length=(size_t)(data_end-event->data);
    if(memchr(event->event,'\r',event_length)!=NULL ||
        memchr(event->event,'\n',event_length)!=NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(!UmiUtf8Validate((const unsigned char *)event->event,event_length,NULL) ||
        !UmiUtf8Validate((const unsigned char *)event->data,data_length,NULL))
        return UMI_STATUS_PARSE_ERROR;
    /* Each input newline can introduce another seven-byte data-field prefix
     * and terminator. Fixed source capacities make the complete staging bound
     * explicit. Caller output may alias the event because publication is last. */
    char staged[sizeof event->data*7U+sizeof event->event+64U];
    int count=snprintf(staged,sizeof staged,"id: %llu\nevent: %s\n",
        (unsigned long long)event->id,event->event);
    if(count<0 || (size_t)count>=sizeof staged)return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t written=(size_t)count,start=0U;
    for(size_t index=0U;index<=data_length;++index){
        if(index<data_length && event->data[index]!='\r' && event->data[index]!='\n')continue;
        count=snprintf(staged+written,sizeof staged-written,"data: %.*s\n",
            (int)(index-start),event->data+start);
        if(count<0 || (size_t)count>=sizeof staged-written)return UMI_STATUS_CAPACITY_EXCEEDED;
        written+=(size_t)count;
        if(index<data_length && event->data[index]=='\r' && index+1U<data_length &&
            event->data[index+1U]=='\n')++index;
        start=index+1U;
    }
    if(written>=sizeof staged-1U)return UMI_STATUS_CAPACITY_EXCEEDED;
    staged[written++]='\n';staged[written]='\0';
    if(capacity<=written)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out_text,staged,written+1U);
    if(out_length!=NULL)*out_length=written;
    return UMI_STATUS_OK;
}

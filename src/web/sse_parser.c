/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/sse_parser.c
 *
 * PURPOSE:
 *   Frame one bounded server-sent event at a time and transfer independent message copies to consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/web/sse_parser.h"
#include "umicom/base/utf8_stream.h"
#include <stdlib.h>
#include <string.h>

#define SSE_STORAGE_LIMIT (1024U*1024U)
struct UmiWebSseParser {
    UmiWebSseLimits limits;
    UmiWebSseState state;
    UmiStatus failure;
    UmiUtf8Stream utf8;
    char *line,*data,*event,*id;
    size_t line_length,data_length,event_length,id_length;
    uint64_t input_bytes,delivered_events,retry_milliseconds;
    int has_retry,has_data,first_scalar,after_cr;
};
static UmiStatus SseFail(UmiWebSseParser *parser,UmiStatus status)
{
    parser->state=UMI_WEB_SSE_FAILED;
    parser->failure=status;
    return status;
}
UmiStatus UmiWebSseParserCreate(const UmiWebSseLimits *limits,UmiWebSseParser **out)
{
    if(limits==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(limits->line_bytes==0U || limits->line_bytes>SSE_STORAGE_LIMIT ||
        limits->data_bytes==0U || limits->data_bytes>SSE_STORAGE_LIMIT ||
        limits->event_bytes<7U || limits->event_bytes>SSE_STORAGE_LIMIT ||
        limits->id_bytes==0U || limits->id_bytes>SSE_STORAGE_LIMIT ||
        limits->input_bytes==0U)return UMI_STATUS_INVALID_ARGUMENT;
    /* All four additions are bounded above before computing the allocation.
     * Storage stays fixed for the response; Feed never grows a buffer. */
    size_t storage=limits->line_bytes+limits->data_bytes+
        limits->event_bytes+limits->id_bytes+4U;
    UmiWebSseParser *parser=calloc(1U,sizeof *parser+storage);
    if(parser==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    parser->limits=*limits;parser->first_scalar=1;
    parser->line=(char *)(parser+1);
    parser->data=parser->line+limits->line_bytes+1U;
    parser->event=parser->data+limits->data_bytes+1U;
    parser->id=parser->event+limits->event_bytes+1U;
    *out=parser;
    return UMI_STATUS_OK;
}
void UmiWebSseParserDestroy(UmiWebSseParser *parser){free(parser);}

static int SseField(const char *field,size_t length,const char *name)
{
    return strlen(name)==length && memcmp(field,name,length)==0;
}
static UmiStatus SseLine(UmiWebSseParser *parser)
{
    if(parser->line_length==0U){
        if(parser->has_data){
            if(parser->event_length==0U){
                memcpy(parser->event,"message",8U);
                parser->event_length=7U;
            }
            parser->state=UMI_WEB_SSE_READY;
        }else{
            /* An event name in a metadata-only block must not leak into the
             * next block. ID and retry are deliberately retained. */
            parser->event_length=0U;parser->event[0]='\0';
        }
        return UMI_STATUS_OK;
    }
    if(parser->line[0]==':')return UMI_STATUS_OK;
    const char *colon=memchr(parser->line,':',parser->line_length);
    size_t field_length=colon?(size_t)(colon-parser->line):parser->line_length;
    const char *value=colon?colon+1:parser->line+parser->line_length;
    size_t length=parser->line_length-(size_t)(value-parser->line);
    if(length>0U && value[0]==' '){++value;--length;}
    if(SseField(parser->line,field_length,"data")){
        size_t separator=parser->has_data?1U:0U;
        size_t remaining=parser->limits.data_bytes-parser->data_length;
        if(separator>remaining || length>remaining-separator)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        if(separator!=0U)parser->data[parser->data_length++]='\n';
        memcpy(parser->data+parser->data_length,value,length);
        parser->data_length+=length;parser->data[parser->data_length]='\0';
        parser->has_data=1;
    }else if(SseField(parser->line,field_length,"event")){
        if(length>parser->limits.event_bytes)return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(parser->event,value,length);parser->event[length]='\0';
        parser->event_length=length;
    }else if(SseField(parser->line,field_length,"id")){
        if(length>parser->limits.id_bytes)return UMI_STATUS_CAPACITY_EXCEEDED;
        memcpy(parser->id,value,length);parser->id[length]='\0';
        parser->id_length=length;
    }else if(SseField(parser->line,field_length,"retry")){
        /* An unrepresentable delay is ignored like an invalid delay rather
         * than wrapped into a shorter value. Transport applies its own policy. */
        uint64_t retry=0U;
        if(length==0U)return UMI_STATUS_OK;
        for(size_t i=0U;i<length;++i){
            if(value[i]<'0' || value[i]>'9')return UMI_STATUS_OK;
            uint64_t digit=(uint64_t)(value[i]-'0');
            if(retry>(UINT64_MAX-digit)/10U)return UMI_STATUS_OK;
            retry=retry*10U+digit;
        }
        parser->retry_milliseconds=retry;parser->has_retry=1;
    }
    return UMI_STATUS_OK;
}
static UmiStatus SseScalar(UmiWebSseParser *parser,const uint8_t *bytes,size_t length)
{
    if(parser->first_scalar){
        parser->first_scalar=0;
        if(length==3U && bytes[0]==0xefU && bytes[1]==0xbbU && bytes[2]==0xbfU)
            return UMI_STATUS_OK;
    }
    if(length==1U && bytes[0]==0U)return UMI_STATUS_PARSE_ERROR;
    if(parser->after_cr){
        parser->after_cr=0;
        if(length==1U && bytes[0]=='\n')return UMI_STATUS_OK;
    }
    if(length==1U && (bytes[0]=='\r' || bytes[0]=='\n')){
        parser->after_cr=bytes[0]=='\r';
        UmiStatus status=SseLine(parser);
        if(status!=UMI_STATUS_OK)return status;
        parser->line_length=0U;parser->line[0]='\0';
        return UMI_STATUS_OK;
    }
    if(length>parser->limits.line_bytes-parser->line_length)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(parser->line+parser->line_length,bytes,length);
    parser->line_length+=length;parser->line[parser->line_length]='\0';
    return UMI_STATUS_OK;
}
UmiStatus UmiWebSseParserFeed(UmiWebSseParser *parser,const void *bytes,
    size_t length,size_t *consumed)
{
    if(parser==NULL || consumed==NULL || (length!=0U && bytes==NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    *consumed=0U;
    if(parser->state==UMI_WEB_SSE_FAILED)return parser->failure;
    if(parser->state==UMI_WEB_SSE_FINISHED)return UMI_STATUS_INVALID_STATE;
    if(parser->state==UMI_WEB_SSE_READY)return UMI_STATUS_OK;
    const uint8_t *input=bytes;
    for(size_t index=0U;index<length;++index){
        if(parser->input_bytes==parser->limits.input_bytes)
            return SseFail(parser,UMI_STATUS_CAPACITY_EXCEEDED);
        ++parser->input_bytes;*consumed=index+1U;
        uint8_t scalar[4];size_t produced=0U;
        UmiStatus status=UmiUtf8StreamAppend(&parser->utf8,input[index],
            scalar,sizeof scalar,&produced);
        if(status!=UMI_STATUS_OK)return SseFail(parser,status);
        if(produced==0U)continue;
        status=SseScalar(parser,scalar,produced);
        if(status!=UMI_STATUS_OK)return SseFail(parser,status);
        if(parser->state==UMI_WEB_SSE_READY)return UMI_STATUS_OK;
    }
    return UMI_STATUS_BUSY;
}
UmiStatus UmiWebSseParserTake(UmiWebSseParser *parser,UmiWebSseMessage **out)
{
    if(parser==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(parser->state==UMI_WEB_SSE_FAILED)return parser->failure;
    if(parser->state==UMI_WEB_SSE_FINISHED)return UMI_STATUS_INVALID_STATE;
    if(parser->state!=UMI_WEB_SSE_READY)return UMI_STATUS_BUSY;
    if(parser->delivered_events==UINT64_MAX)return SseFail(parser,UMI_STATUS_CAPACITY_EXCEEDED);
    /* No application callbacks run inside the parser. The copied message may
     * cross a queue boundary and outlive both this response and its transport. */
    size_t storage=parser->event_length+parser->data_length+parser->id_length+3U;
    UmiWebSseMessage *message=malloc(sizeof *message+storage);
    if(message==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    char *cursor=(char *)(message+1);
    message->event=cursor;memcpy(cursor,parser->event,parser->event_length+1U);
    cursor+=parser->event_length+1U;
    message->data=cursor;memcpy(cursor,parser->data,parser->data_length+1U);
    cursor+=parser->data_length+1U;
    message->id=cursor;memcpy(cursor,parser->id,parser->id_length+1U);
    message->event_bytes=parser->event_length;message->data_bytes=parser->data_length;
    message->id_bytes=parser->id_length;message->has_retry=parser->has_retry;
    message->retry_milliseconds=parser->retry_milliseconds;
    parser->has_data=0;parser->data_length=0U;parser->data[0]='\0';
    parser->event_length=0U;parser->event[0]='\0';
    ++parser->delivered_events;parser->state=UMI_WEB_SSE_ACTIVE;
    *out=message;
    return UMI_STATUS_OK;
}
void UmiWebSseMessageFree(UmiWebSseMessage *message){free(message);}
UmiStatus UmiWebSseParserFinish(UmiWebSseParser *parser)
{
    if(parser==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(parser->state==UMI_WEB_SSE_FAILED)return parser->failure;
    if(parser->state==UMI_WEB_SSE_FINISHED)return UMI_STATUS_OK;
    if(parser->state==UMI_WEB_SSE_READY)return UMI_STATUS_BUSY;
    if(UmiUtf8StreamFinish(&parser->utf8)!=UMI_STATUS_OK)
        return SseFail(parser,UMI_STATUS_PARSE_ERROR);
    parser->line_length=parser->data_length=parser->event_length=0U;
    parser->line[0]=parser->data[0]=parser->event[0]='\0';
    parser->has_data=0;parser->state=UMI_WEB_SSE_FINISHED;
    return UMI_STATUS_OK;
}
UmiStatus UmiWebSseParserInspect(const UmiWebSseParser *parser,UmiWebSseSnapshot *out)
{
    if(parser==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=(UmiWebSseSnapshot){parser->state,parser->failure,parser->input_bytes,
        parser->delivered_events,parser->line_length,parser->data_length,
        parser->utf8.pendingCount,parser->retry_milliseconds,parser->has_retry};
    return UMI_STATUS_OK;
}

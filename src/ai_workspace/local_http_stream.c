/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_http_stream.c
 *
 * PURPOSE:
 *   Feed admitted local HTTP event bytes into the shared model sequencer and preserve first failure evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_http_stream_internal.h"
#include "local_http_internal.h"
#include <string.h>
#ifdef UMICOM_AI_WORKSPACE_HAS_HTTP
typedef struct AwHttpStreamContext {
    UmiWebSseResponse *response;
    AwLocalChatStream *stream;
    const UmiAiWorkspaceCancellation *cancellation;
    UmiStatus failure;
} AwHttpStreamContext;
static UmiStatus CallbackReady(AwHttpStreamContext *context,size_t size,size_t count,size_t *bytes)
{
    if(context->failure!=UMI_STATUS_OK)return context->failure;
    if(size!=0U && count>SIZE_MAX/size)return context->failure=UMI_STATUS_CAPACITY_EXCEEDED;
    if(UmiAiWorkspaceCancellationIsRequested(context->cancellation))return context->failure=UMI_STATUS_CANCELLED;
    *bytes=size*count;return UMI_STATUS_OK;
}
static size_t StreamHeader(char *data,size_t size,size_t count,void *opaque)
{
    AwHttpStreamContext *context=opaque;size_t bytes=0U;
    if(CallbackReady(context,size,count,&bytes)!=UMI_STATUS_OK)return 0U;
    if(bytes==0U)return 0U;
    context->failure=UmiWebSseResponseHeader(context->response,data,bytes);
    return context->failure==UMI_STATUS_OK?bytes:0U;
}
static size_t StreamWrite(char *data,size_t size,size_t count,void *opaque)
{
    AwHttpStreamContext *context=opaque;size_t bytes=0U;
    if(CallbackReady(context,size,count,&bytes)!=UMI_STATUS_OK)return 0U;
    context->failure=UmiWebSseResponseBody(context->response,bytes);
    if(context->failure==UMI_STATUS_OK)context->failure=AwLocalChatStreamFeed(context->stream,data,bytes);
    return context->failure==UMI_STATUS_OK?bytes:0U;
}
static int StreamProgress(void *opaque,curl_off_t a,curl_off_t b,curl_off_t c,curl_off_t d)
{
    AwHttpStreamContext *context=opaque;(void)a;(void)b;(void)c;(void)d;
    if(context->failure!=UMI_STATUS_OK)return 1;
    if(UmiAiWorkspaceCancellationIsRequested(context->cancellation)){
        context->failure=UMI_STATUS_CANCELLED;return 1;
    }
    return 0;
}
UmiStatus AwLocalChatExchangeStream(uint16_t port,uint32_t timeout_millis,
    const char *encoded_request,uint64_t wire_limit,
    const UmiAiWorkspaceCancellation *cancellation,AwLocalChatStream *stream,
    AwLocalHttpStreamResult *out_result)
{
    if(port<1024U || timeout_millis<100U || timeout_millis>60000U ||
        encoded_request==NULL || wire_limit==0U || stream==NULL || out_result==NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Copy once to verify the caller supplied a fresh protocol owner. The
     * allocation is released before transport setup and publishes no output. */
    AwLocalChatStreamSnapshot initial;char *initial_text=NULL;
    UmiStatus status=AwLocalChatStreamCopy(stream,&initial_text,&initial);
    if(status!=UMI_STATUS_OK)return status;
    UmiAiResponseStreamTextFree(initial_text);
    if(initial.phase!=AW_LOCAL_CHAT_ACTIVE || initial.text.last_sequence!=0U ||
        initial.completion_id[0]!='\0')return UMI_STATUS_INVALID_STATE;
    AwHttpStreamContext context={.stream=stream,.cancellation=cancellation};
    AwLocalHttpStreamResult result={0};
    CURL *curl=NULL;struct curl_slist *headers=NULL;
    if(UmiAiWorkspaceCancellationIsRequested(cancellation)){status=UMI_STATUS_CANCELLED;goto terminal;}
    UmiWebSseResponseLimits limits={8192U,65536U,256U,8U,wire_limit};
    status=UmiWebSseResponseCreate(&limits,&context.response);
    if(status!=UMI_STATUS_OK)goto terminal;
    curl=curl_easy_init();
    if(curl==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto terminal;}
    headers=curl_slist_append(NULL,"Content-Type: application/json");
    if(headers==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto terminal;}
    struct curl_slist *next=curl_slist_append(headers,"Accept: text/event-stream");
    if(next==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto terminal;}headers=next;
    const AwLocalHttpCallbacks callbacks={.write=StreamWrite,.write_context=&context,
        .header=StreamHeader,.header_context=&context,.progress=StreamProgress,.progress_context=&context};
    status=AwLocalHttpConfigure(curl,port,timeout_millis,encoded_request,headers,&callbacks,true);
    if(status!=UMI_STATUS_OK)goto terminal;
    CURLcode code=curl_easy_perform(curl);
    /* Preserve the first semantic callback error over curl's generic aborted
     * write status. Cancellation is checked again at the publication boundary. */
    if(context.failure!=UMI_STATUS_OK)status=context.failure;
    else if(UmiAiWorkspaceCancellationIsRequested(cancellation))status=UMI_STATUS_CANCELLED;
    else if(code==CURLE_OPERATION_TIMEDOUT)status=UMI_STATUS_TIMEOUT;
    else if(code!=CURLE_OK)status=UMI_STATUS_IO_ERROR;
    else status=UmiWebSseResponseFinish(context.response);
    if(status==UMI_STATUS_OK)status=AwLocalChatStreamFinish(stream);
terminal:
    if(status==UMI_STATUS_CANCELLED){
        UmiStatus cancelled=AwLocalChatStreamCancel(stream);
        if(cancelled!=UMI_STATUS_OK)status=cancelled;
    }else if(status!=UMI_STATUS_OK){
        /* The sequencer retains its own original failure if a payload already
         * failed. Its separate terminal-publication status remains inspectable. */
        status=AwLocalChatStreamFail(stream,status);
    }
/* Retain borrowed header storage until the easy handle has finished its cleanup callbacks; response and stream owners remain alive throughout cleanup. The former implementation is retained for owner review. */
#if 0
    if(headers!=NULL)curl_slist_free_all(headers);
    if(curl!=NULL)curl_easy_cleanup(curl);
#endif
    if(curl!=NULL)curl_easy_cleanup(curl);
    if(headers!=NULL)curl_slist_free_all(headers);
    if(context.response!=NULL){
        UmiStatus inspected=UmiWebSseResponseInspect(context.response,&result.response);
        if(status==UMI_STATUS_OK && inspected!=UMI_STATUS_OK)status=inspected;
    }
    UmiWebSseResponseDestroy(context.response);
    result.status=status;*out_result=result;return status;
}
#else
UmiStatus AwLocalChatExchangeStream(uint16_t port,uint32_t timeout_millis,
    const char *encoded_request,uint64_t wire_limit,
    const UmiAiWorkspaceCancellation *cancellation,AwLocalChatStream *stream,
    AwLocalHttpStreamResult *out_result)
{
    (void)port;(void)timeout_millis;(void)encoded_request;(void)wire_limit;
    (void)cancellation;(void)stream;(void)out_result;
    return UMI_STATUS_UNAVAILABLE;
}
#endif

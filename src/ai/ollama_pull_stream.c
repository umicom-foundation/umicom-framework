/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/ollama_pull_stream.c
 *
 * PURPOSE:
 *   Own bounded newline-delimited model progress and require explicit error-free end of input.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_pull.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>

struct UmiOllamaPullStream {
    UmiOllamaPullObserver observer;
    void *context;
    UmiOllamaPullSnapshot view;
    unsigned char line[UMI_OLLAMA_PULL_LINE_BYTES];
    int busy;
};
static UmiStatus PullFail(UmiOllamaPullStream *stream,UmiStatus status)
{
    if(stream->view.failure==UMI_STATUS_OK)stream->view.failure=status;
    umi_secret_clear(stream->line,sizeof stream->line);stream->view.pending_bytes=0U;
    return stream->view.failure;
}
/* Framing belongs to the stream owner, while object semantics remain in the
 * complete-event decoder. Delivered prefix events survive later failures. */
static UmiStatus PullLine(UmiOllamaPullStream *stream,const UmiCancellationToken *cancel)
{
    size_t length=stream->view.pending_bytes;
    if(length!=0U && stream->line[length-1U]=='\r')--length;
    if(length==0U){stream->view.pending_bytes=0U;return UMI_STATUS_OK;}
    if(stream->view.terminal_seen)return PullFail(stream,UMI_STATUS_PARSE_ERROR);
    if(stream->view.events==UMI_OLLAMA_PULL_MAX_EVENTS)return PullFail(stream,UMI_STATUS_CAPACITY_EXCEEDED);
    UmiOllamaPullEvent event={0};
    UmiStatus status=UmiOllamaPullEventDecode(stream->line,length,cancel,&event);
    if(status!=UMI_STATUS_OK)return PullFail(stream,status);
    umi_secret_clear(stream->line,stream->view.pending_bytes);stream->view.pending_bytes=0U;
    umi_secret_clear(&stream->view.last,sizeof stream->view.last);stream->view.last=event;
    stream->view.has_event=1;++stream->view.events;
    stream->view.terminal_seen=event.kind!=UMI_OLLAMA_PULL_STATUS;
    if(stream->observer!=NULL)status=stream->observer(&event,stream->context);
    umi_secret_clear(&event,sizeof event);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    return status==UMI_STATUS_OK?UMI_STATUS_OK:PullFail(stream,status);
}
UmiStatus UmiOllamaPullStreamCreate(UmiOllamaPullObserver observer,void *context,UmiOllamaPullStream **out)
{
    if(out==NULL || *out!=NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiOllamaPullStream *stream=calloc(1U,sizeof *stream);
    if(stream==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    stream->observer=observer;stream->context=context;*out=stream;return UMI_STATUS_OK;
}
UmiStatus UmiOllamaPullStreamFeed(UmiOllamaPullStream *stream,const void *bytes,
    size_t length,const UmiCancellationToken *cancel)
{
    if(stream==NULL || (bytes==NULL && length!=0U))return UMI_STATUS_INVALID_ARGUMENT;
    if(stream->busy)return UMI_STATUS_BUSY;
    if(stream->view.failure!=UMI_STATUS_OK)return stream->view.failure;
    if(stream->view.input_finished)return UMI_STATUS_INVALID_STATE;
    if(umi_cancellation_token_is_requested(cancel))return PullFail(stream,UMI_STATUS_CANCELLED);
    if(length>UMI_OLLAMA_PULL_STREAM_BYTES-stream->view.bytes_admitted)
        return PullFail(stream,UMI_STATUS_CAPACITY_EXCEEDED);
    stream->view.bytes_admitted+=length;stream->busy=1;
    const unsigned char *input=bytes;UmiStatus status=UMI_STATUS_OK;
    for(size_t i=0U;i<length;++i){
        if(umi_cancellation_token_is_requested(cancel)){status=PullFail(stream,UMI_STATUS_CANCELLED);break;}
        if(input[i]=='\n')status=PullLine(stream,cancel);
        else if(stream->view.pending_bytes==sizeof stream->line)status=PullFail(stream,UMI_STATUS_CAPACITY_EXCEEDED);
        else stream->line[stream->view.pending_bytes++]=input[i];
        if(status!=UMI_STATUS_OK)break;
    }
    stream->busy=0;return status;
}
UmiStatus UmiOllamaPullStreamFinish(UmiOllamaPullStream *stream,const UmiCancellationToken *cancel)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(stream->busy)return UMI_STATUS_BUSY;
    if(stream->view.failure!=UMI_STATUS_OK)return stream->view.failure;
    if(stream->view.input_finished)return UMI_STATUS_OK;
    stream->view.input_finished=1;
    if(umi_cancellation_token_is_requested(cancel))return PullFail(stream,UMI_STATUS_CANCELLED);
    stream->busy=1;
    UmiStatus status=stream->view.pending_bytes==0U?UMI_STATUS_OK:PullLine(stream,cancel);
    if(status==UMI_STATUS_OK && !stream->view.terminal_seen)status=PullFail(stream,UMI_STATUS_PARSE_ERROR);
    if(status==UMI_STATUS_OK && stream->view.last.kind==UMI_OLLAMA_PULL_ERROR)status=PullFail(stream,UMI_STATUS_IO_ERROR);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=PullFail(stream,UMI_STATUS_CANCELLED);
    stream->busy=0;return status;
}
UmiStatus UmiOllamaPullStreamRead(const UmiOllamaPullStream *stream,UmiOllamaPullSnapshot *out)
{
    if(stream==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=stream->view;return UMI_STATUS_OK;
}
UmiStatus UmiOllamaPullStreamRelease(UmiOllamaPullStream **owner)
{
    if(owner==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(*owner==NULL)return UMI_STATUS_OK;
    if((*owner)->busy)return UMI_STATUS_BUSY;
    umi_secret_clear(*owner,sizeof **owner);free(*owner);*owner=NULL;return UMI_STATUS_OK;
}

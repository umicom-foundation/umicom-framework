/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_stream.c
 *
 * PURPOSE:
 *   Own encoded local requests, cooperative cancellation and joined background streaming lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/local_stream.h"
#include "umicom/platform/threading.h"
#include "local_transport_internal.h"
#include "local_http_stream_internal.h"
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiLocalStream {
    uint64_t owner_thread;
    UmiAiLocalTransport *transport;
    UmiAiLocalStreamOptions options;
    char *encoded;
    AwLocalChatStream *protocol;
    UmiAiWorkspaceCancellation *cancellation;
    UmiThread *thread;
    bool started,joined;
    atomic_bool completed;
    UmiAiLocalStreamResult result;
};
static UmiStatus Owner(UmiAiLocalStream *stream)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    return stream->owner_thread==umi_thread_current_id()?UMI_STATUS_OK:UMI_STATUS_INVALID_STATE;
}
/* Only the producer (or the owner before any producer starts) writes protocol
 * metadata. Release publication makes the complete immutable result visible.
 * Text has its own mutex so readers need not wait for this publication. */
static void Complete(UmiAiLocalStream *stream,UmiStatus status,
    const AwLocalHttpStreamResult *http)
{
    UmiAiLocalStreamResult result={.operation_status=status};
    if(http!=NULL)result.response=http->response;
    AwLocalChatStreamSnapshot protocol;
    result.observation_status=AwLocalChatStreamInspect(stream->protocol,&protocol);
    if(result.observation_status==UMI_STATUS_OK){
        result.terminal_publication_status=protocol.terminal_publication_status;
        result.text=protocol.text;result.finish_reason=protocol.finish_reason;
        result.usage=protocol.usage;result.has_usage=protocol.has_usage;
        memcpy(result.completion_id,protocol.completion_id,sizeof result.completion_id);
    }
    stream->result=result;
    atomic_store_explicit(&stream->completed,true,memory_order_release);
}
static int Run(void *opaque)
{
    UmiAiLocalStream *stream=opaque;
    AwLocalHttpStreamResult http={0};
    UmiStatus status=AwLocalChatExchangeStream(stream->options.port,stream->options.timeout_millis,
        stream->encoded,stream->options.maximum_wire_bytes,stream->cancellation,stream->protocol,&http);
    /* Even a pre-transfer observation/allocation failure must retire text. The
     * sequencer keeps an earlier failure instead of overwriting its evidence. */
    if(status==UMI_STATUS_CANCELLED)(void)AwLocalChatStreamCancel(stream->protocol);
    else if(status!=UMI_STATUS_OK)(void)AwLocalChatStreamFail(stream->protocol,status);
    Complete(stream,status,&http);return 0;
}
static void FreeStorage(UmiAiLocalStream *stream)
{
    AwLocalChatFree(stream->encoded);AwLocalChatStreamDestroy(stream->protocol);
    UmiAiWorkspaceCancellationDestroy(stream->cancellation);free(stream);
}
UmiStatus UmiAiLocalStreamCreate(UmiAiLocalTransport *transport,
    const UmiAiRequest *request,const UmiAiLocalStreamOptions *options,UmiAiLocalStream **out)
{
    if(transport==NULL || request==NULL || options==NULL || out==NULL ||
        options->port<1024U || options->timeout_millis<100U || options->timeout_millis>60000U ||
        options->maximum_text_bytes==0U || options->maximum_text_bytes>UMI_AI_RESPONSE_STREAM_MAX_BYTES ||
        options->maximum_wire_bytes==0U || options->maximum_wire_bytes>UMI_AI_LOCAL_STREAM_MAX_WIRE_BYTES ||
        options->identity.request_id==0U || options->identity.model_revision==0U)return UMI_STATUS_INVALID_ARGUMENT;
    if(!options->allow_network)return UMI_STATUS_PERMISSION_DENIED;
    UmiStatus status=AwLocalTransportRetain(transport);
    if(status!=UMI_STATUS_OK)return status;
    UmiAiLocalStream *stream=calloc(1U,sizeof *stream);
    if(stream==NULL){(void)AwLocalTransportRelease(transport);return UMI_STATUS_OUT_OF_MEMORY;}
    stream->owner_thread=umi_thread_current_id();stream->transport=transport;stream->options=*options;
    atomic_init(&stream->completed,false);
    status=AwLocalChatEncodeStreaming(request,&stream->encoded);
    if(status==UMI_STATUS_OK)status=AwLocalChatStreamCreate(request,options->identity,
        options->maximum_text_bytes,options->maximum_wire_bytes,&stream->protocol);
    if(status==UMI_STATUS_OK)status=UmiAiWorkspaceCancellationCreate(&stream->cancellation);
    if(status!=UMI_STATUS_OK){
        FreeStorage(stream);(void)AwLocalTransportRelease(transport);return status;
    }
    *out=stream;return UMI_STATUS_OK;
}
UmiStatus UmiAiLocalStreamStart(UmiAiLocalStream *stream)
{
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    if(stream->started || atomic_load_explicit(&stream->completed,memory_order_acquire))
        return UMI_STATUS_INVALID_STATE;
    status=umi_thread_start(Run,stream,&stream->thread);
    if(status==UMI_STATUS_OK)stream->started=true;
    return status;
}
UmiStatus UmiAiLocalStreamCancel(UmiAiLocalStream *stream)
{
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    if(atomic_load_explicit(&stream->completed,memory_order_acquire))return UMI_STATUS_OK;
    UmiAiWorkspaceCancellationRequest(stream->cancellation);
    if(!stream->started){
        status=AwLocalChatStreamCancel(stream->protocol);
        Complete(stream,status==UMI_STATUS_OK?UMI_STATUS_CANCELLED:status,NULL);
    }
    return status;
}
UmiStatus UmiAiLocalStreamPoll(UmiAiLocalStream *stream,UmiAiLocalStreamSnapshot *out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    bool complete=atomic_load_explicit(&stream->completed,memory_order_acquire);
    UmiAiLocalStreamSnapshot snapshot={
        .state=stream->joined?UMI_AI_LOCAL_STREAM_JOINED:complete?UMI_AI_LOCAL_STREAM_FINISHED:
            stream->started?UMI_AI_LOCAL_STREAM_RUNNING:UMI_AI_LOCAL_STREAM_CREATED,
        .operation_status=complete?stream->result.operation_status:UMI_STATUS_BUSY,
        .cancellation_requested=UmiAiWorkspaceCancellationIsRequested(stream->cancellation)};
    *out=snapshot;return UMI_STATUS_OK;
}
UmiStatus UmiAiLocalStreamCopyText(UmiAiLocalStream *stream,char **out_text,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    return AwLocalChatStreamTextCopy(stream->protocol,out_text,out_snapshot);
}
UmiStatus UmiAiLocalStreamReadResult(UmiAiLocalStream *stream,UmiAiLocalStreamResult *out)
{
    if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    if(!atomic_load_explicit(&stream->completed,memory_order_acquire))return UMI_STATUS_BUSY;
    *out=stream->result;return UMI_STATUS_OK;
}
UmiStatus UmiAiLocalStreamTryJoin(UmiAiLocalStream *stream)
{
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    if(stream->joined || stream->thread==NULL)return UMI_STATUS_OK;
    status=UmiThreadTryJoin(stream->thread);
    if(status==UMI_STATUS_OK)stream->joined=true;
    return status;
}
UmiStatus UmiAiLocalStreamJoin(UmiAiLocalStream *stream)
{
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    if(stream->joined || stream->thread==NULL)return UMI_STATUS_OK;
    int result=0;
    status=umi_thread_join(stream->thread,&result);
    if(status==UMI_STATUS_OK)stream->joined=true;
    return status;
}
UmiStatus UmiAiLocalStreamDestroy(UmiAiLocalStream **in_out)
{
    if(in_out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiLocalStream *stream=*in_out;if(stream==NULL)return UMI_STATUS_OK;
    UmiStatus status=Owner(stream);if(status!=UMI_STATUS_OK)return status;
    if(stream->thread!=NULL && !stream->joined)return UMI_STATUS_BUSY;
    status=UmiThreadRelease(&stream->thread);if(status!=UMI_STATUS_OK)return status;
    status=AwLocalTransportRelease(stream->transport);if(status!=UMI_STATUS_OK)return status;
    FreeStorage(stream);*in_out=NULL;return UMI_STATUS_OK;
}

/* Preserve the controller ownership check and delegate byte boundaries to
 * the shared response-stream owner; do not read producer protocol metadata. */
UmiStatus UmiAiLocalStreamCopyTextRange(UmiAiLocalStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    UmiStatus status = Owner(stream);
    if (status != UMI_STATUS_OK) return status;
    return AwLocalChatStreamTextCopyRange(stream->protocol, offset, maximum_bytes, out_text, out_copied, out_snapshot);
}

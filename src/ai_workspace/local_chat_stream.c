/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/local_chat_stream.c
 *
 * PURPOSE:
 *   Require coherent provider identity and terminal evidence before completing retained AI text.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "local_chat_stream_internal.h"
#include "workspace_internal.h"
#include "umicom/web/sse_parser.h"
#include <stdlib.h>
#include <string.h>
struct AwLocalChatStream {
    UmiWebSseParser *parser;
    UmiAiResponseStream *text;
    /* The existing codec consumes this ABI type. Only the two identity arrays
     * are populated; original prompts and messages are never retained here. */
    UmiAiRequest request;
    UmiAiResponseStreamIdentity identity;
    AwLocalChatStreamPhase phase;
    UmiStatus failure,terminal_publication_status;
    uint64_t sequence;
    char completion_id[UMI_AI_ID_CAPACITY];
    UmiAiFinishReason finish_reason;
    UmiAiUsage usage;
    bool has_usage;
};
/* Read the committed text sequence before publishing failure because an unlock error can follow a successful append. The former implementation is retained for owner review. */
#if 0
static UmiStatus AwStreamFail(AwLocalChatStream *stream,UmiStatus failure)
{
    stream->failure=failure;stream->phase=AW_LOCAL_CHAT_FAILED;
    UmiAiResponseStreamSnapshot snapshot;
    UmiStatus published=UmiAiResponseStreamInspect(stream->text,&snapshot);
    if(published==UMI_STATUS_OK){
        if(snapshot.state==UMI_AI_RESPONSE_STREAM_ACTIVE && stream->sequence<UINT64_MAX)
            published=UmiAiResponseStreamFail(stream->text,stream->identity,stream->sequence+1U,failure);
        else if(snapshot.state!=UMI_AI_RESPONSE_STREAM_FAILED)
            published=UMI_STATUS_INVALID_STATE;
    }
    stream->terminal_publication_status=published;
    return failure;
}
#endif
static UmiStatus AwStreamFail(AwLocalChatStream *stream,UmiStatus failure)
{
    stream->failure=failure;stream->phase=AW_LOCAL_CHAT_FAILED;
    UmiAiResponseStreamSnapshot snapshot;
    UmiStatus published=UmiAiResponseStreamInspect(stream->text,&snapshot);
    if(published==UMI_STATUS_OK){
        if(snapshot.state==UMI_AI_RESPONSE_STREAM_ACTIVE && snapshot.last_sequence<UINT64_MAX)
            published=UmiAiResponseStreamFail(stream->text,stream->identity,snapshot.last_sequence+1U,failure);
        else if(snapshot.state!=UMI_AI_RESPONSE_STREAM_FAILED)
            published=UMI_STATUS_INVALID_STATE;
    }
    stream->terminal_publication_status=published;
    return failure;
}
UmiStatus AwLocalChatStreamCreate(const UmiAiRequest *request,
    UmiAiResponseStreamIdentity identity,size_t text_limit,uint64_t wire_limit,
    AwLocalChatStream **out)
{
    if(out==NULL || request==NULL || wire_limit==0U)return UMI_STATUS_INVALID_ARGUMENT;
    AwLocalChatDelta marker;
    UmiStatus status=AwLocalChatDeltaDecode(request,"[DONE]",6U,&marker);
    if(status!=UMI_STATUS_OK)return status;
    AwLocalChatStream *stream=calloc(1U,sizeof *stream);
    if(stream==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    status=UmiAiResponseStreamCreate(identity,text_limit,&stream->text);
    if(status!=UMI_STATUS_OK){free(stream);return status;}
    UmiWebSseLimits limits={AW_HTTP_BODY_CAPACITY-1U,AW_HTTP_BODY_CAPACITY-1U,
        UMI_AI_ID_CAPACITY-1U,UMI_AI_ID_CAPACITY-1U,wire_limit};
    status=UmiWebSseParserCreate(&limits,&stream->parser);
    if(status!=UMI_STATUS_OK){UmiAiResponseStreamDestroy(stream->text);free(stream);return status;}
    stream->identity=identity;
    memcpy(stream->request.request_id,request->request_id,strlen(request->request_id)+1U);
    memcpy(stream->request.model_id,request->model_id,strlen(request->model_id)+1U);
    *out=stream;return UMI_STATUS_OK;
}
void AwLocalChatStreamDestroy(AwLocalChatStream *stream)
{
    if(stream==NULL)return;
    UmiWebSseParserDestroy(stream->parser);
    UmiAiResponseStreamDestroy(stream->text);
    free(stream);
}
/* Require the codec zero-choice usage tag after finish to reject a second choice record that happens to contain no text. The former implementation is retained for owner review. */
#if 0
static UmiStatus AwStreamAccept(AwLocalChatStream *stream,const UmiWebSseMessage *message)
{
    if(strcmp(message->event,"message")!=0)return UMI_STATUS_UNAVAILABLE;
    AwLocalChatDelta delta;
    UmiStatus status=AwLocalChatDeltaDecode(&stream->request,message->data,message->data_bytes,&delta);
    if(status!=UMI_STATUS_OK)return status;
    if(delta.done){
        if(stream->phase!=AW_LOCAL_CHAT_FINISH_REPORTED)return UMI_STATUS_PARSE_ERROR;
        stream->phase=AW_LOCAL_CHAT_MARKER;return UMI_STATUS_OK;
    }
    if(stream->completion_id[0]!='\0' &&
        strcmp(stream->completion_id,delta.completion_id)!=0)return UMI_STATUS_PARSE_ERROR;
    if(stream->phase==AW_LOCAL_CHAT_FINISH_REPORTED){
        /* A final usage-only chunk is allowed once. No further text, role,
         * finish or empty metadata chunk can revise the finished choice. */
        if(!delta.has_usage || stream->has_usage || delta.has_content ||
            delta.has_role || delta.has_finish)return UMI_STATUS_PARSE_ERROR;
    }else if(stream->phase!=AW_LOCAL_CHAT_ACTIVE ||
        (delta.has_usage && !delta.has_finish))return UMI_STATUS_PARSE_ERROR;
    if(delta.has_content){
        if(stream->sequence>=UINT64_MAX-1U)return UMI_STATUS_CAPACITY_EXCEEDED;
        status=UmiAiResponseStreamPush(stream->text,stream->identity,stream->sequence+1U,
            (const unsigned char *)delta.text,strlen(delta.text),0);
        if(status!=UMI_STATUS_OK)return status;
        ++stream->sequence;
    }
    /* Publish protocol metadata only after text admission succeeds. The
     * terminal marker and successful transport EOF remain separate steps. */
    if(stream->completion_id[0]=='\0')
        memcpy(stream->completion_id,delta.completion_id,strlen(delta.completion_id)+1U);
    if(delta.has_usage){stream->usage=delta.usage;stream->has_usage=true;}
    if(delta.has_finish){
        stream->finish_reason=delta.finish_reason;
        stream->phase=AW_LOCAL_CHAT_FINISH_REPORTED;
    }
    return UMI_STATUS_OK;
}
#endif
static UmiStatus AwStreamAccept(AwLocalChatStream *stream,const UmiWebSseMessage *message)
{
    if(strcmp(message->event,"message")!=0)return UMI_STATUS_UNAVAILABLE;
    AwLocalChatDelta delta;
    UmiStatus status=AwLocalChatDeltaDecode(&stream->request,message->data,message->data_bytes,&delta);
    if(status!=UMI_STATUS_OK)return status;
    if(delta.done){
        if(stream->phase!=AW_LOCAL_CHAT_FINISH_REPORTED)return UMI_STATUS_PARSE_ERROR;
        stream->phase=AW_LOCAL_CHAT_MARKER;return UMI_STATUS_OK;
    }
    if(stream->completion_id[0]!='\0' &&
        strcmp(stream->completion_id,delta.completion_id)!=0)return UMI_STATUS_PARSE_ERROR;
    if(stream->phase==AW_LOCAL_CHAT_FINISH_REPORTED){
        /* A final usage-only chunk is allowed once. No further text, role,
         * finish or empty metadata chunk can revise the finished choice. */
        if(!delta.usage_only || !delta.has_usage || stream->has_usage || delta.has_content ||
            delta.has_role || delta.has_finish)return UMI_STATUS_PARSE_ERROR;
    }else if(stream->phase!=AW_LOCAL_CHAT_ACTIVE ||
        (delta.has_usage && !delta.has_finish))return UMI_STATUS_PARSE_ERROR;
    if(delta.has_content){
        if(stream->sequence>=UINT64_MAX-1U)return UMI_STATUS_CAPACITY_EXCEEDED;
        status=UmiAiResponseStreamPush(stream->text,stream->identity,stream->sequence+1U,
            (const unsigned char *)delta.text,strlen(delta.text),0);
        if(status!=UMI_STATUS_OK)return status;
        ++stream->sequence;
    }
    /* Publish protocol metadata only after text admission succeeds. The
     * terminal marker and successful transport EOF remain separate steps. */
    if(stream->completion_id[0]=='\0')
        memcpy(stream->completion_id,delta.completion_id,strlen(delta.completion_id)+1U);
    if(delta.has_usage){stream->usage=delta.usage;stream->has_usage=true;}
    if(delta.has_finish){
        stream->finish_reason=delta.finish_reason;
        stream->phase=AW_LOCAL_CHAT_FINISH_REPORTED;
    }
    return UMI_STATUS_OK;
}
UmiStatus AwLocalChatStreamFeed(AwLocalChatStream *stream,const void *bytes,size_t length)
{
    if(stream==NULL || (bytes==NULL && length!=0U))return UMI_STATUS_INVALID_ARGUMENT;
    if(stream->phase==AW_LOCAL_CHAT_FAILED)return stream->failure;
    if(stream->phase==AW_LOCAL_CHAT_CANCELLED)return UMI_STATUS_CANCELLED;
    if(stream->phase==AW_LOCAL_CHAT_COMPLETE)return UMI_STATUS_INVALID_STATE;
    const unsigned char *input=bytes;size_t offset=0U;
    while(offset<length){
        if(stream->phase==AW_LOCAL_CHAT_MARKER){
            for(size_t i=offset;i<length;++i)
                if(input[i]!=' ' && input[i]!='\t' && input[i]!='\r' && input[i]!='\n')
                    return AwStreamFail(stream,UMI_STATUS_PARSE_ERROR);
        }
        size_t consumed=0U;
        UmiStatus status=UmiWebSseParserFeed(stream->parser,input+offset,length-offset,&consumed);
        offset+=consumed;
        if(status==UMI_STATUS_OK){
            UmiWebSseMessage *message=NULL;
            status=UmiWebSseParserTake(stream->parser,&message);
            if(status!=UMI_STATUS_OK)return AwStreamFail(stream,status);
            status=AwStreamAccept(stream,message);
            UmiWebSseMessageFree(message);
            if(status!=UMI_STATUS_OK)return AwStreamFail(stream,status);
        }else if(status!=UMI_STATUS_BUSY)return AwStreamFail(stream,status);
        else if(offset!=length)return AwStreamFail(stream,UMI_STATUS_INTERNAL_ERROR);
    }
    return UMI_STATUS_OK;
}
UmiStatus AwLocalChatStreamFinish(AwLocalChatStream *stream)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(stream->phase==AW_LOCAL_CHAT_FAILED)return stream->failure;
    if(stream->phase==AW_LOCAL_CHAT_CANCELLED)return UMI_STATUS_CANCELLED;
    if(stream->phase==AW_LOCAL_CHAT_COMPLETE)return UMI_STATUS_OK;
    if(stream->phase!=AW_LOCAL_CHAT_MARKER)return AwStreamFail(stream,UMI_STATUS_PARSE_ERROR);
    UmiStatus status=UmiWebSseParserFinish(stream->parser);
    if(status!=UMI_STATUS_OK)return AwStreamFail(stream,status);
    if(stream->sequence==UINT64_MAX)return AwStreamFail(stream,UMI_STATUS_CAPACITY_EXCEEDED);
    status=UmiAiResponseStreamPush(stream->text,stream->identity,stream->sequence+1U,NULL,0U,1);
    if(status!=UMI_STATUS_OK)return AwStreamFail(stream,status);
    ++stream->sequence;stream->phase=AW_LOCAL_CHAT_COMPLETE;
    return UMI_STATUS_OK;
}
UmiStatus AwLocalChatStreamCancel(AwLocalChatStream *stream)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(stream->phase==AW_LOCAL_CHAT_CANCELLED)return UMI_STATUS_OK;
    if(stream->phase==AW_LOCAL_CHAT_FAILED)return stream->failure;
    if(stream->phase==AW_LOCAL_CHAT_COMPLETE)return UMI_STATUS_INVALID_STATE;
    UmiStatus status=UmiAiResponseStreamCancel(stream->text,stream->identity);
    if(status!=UMI_STATUS_OK)return AwStreamFail(stream,status);
    stream->phase=AW_LOCAL_CHAT_CANCELLED;stream->failure=UMI_STATUS_CANCELLED;
    return UMI_STATUS_OK;
}
UmiStatus AwLocalChatStreamFail(AwLocalChatStream *stream,UmiStatus failure)
{
    if(stream==NULL || failure<=UMI_STATUS_OK || failure>=UMI_STATUS_BUSY ||
        failure==UMI_STATUS_CANCELLED)return UMI_STATUS_INVALID_ARGUMENT;
    if(stream->phase==AW_LOCAL_CHAT_FAILED)return stream->failure;
    if(stream->phase==AW_LOCAL_CHAT_CANCELLED)return UMI_STATUS_CANCELLED;
    if(stream->phase==AW_LOCAL_CHAT_COMPLETE)return UMI_STATUS_INVALID_STATE;
    return AwStreamFail(stream,failure);
}
/* Retain serialized protocol observations while adding a text-only path safe for background producer observation. The former implementation is retained for owner review. */
#if 0
UmiStatus AwLocalChatStreamCopy(AwLocalChatStream *stream,char **out_text,
    AwLocalChatStreamSnapshot *out_snapshot)
{
    if(stream==NULL || out_text==NULL || out_snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    AwLocalChatStreamSnapshot snapshot={0};char *text=NULL;
    UmiStatus status=UmiAiResponseStreamCopy(stream->text,&text,&snapshot.text);
    if(status!=UMI_STATUS_OK)return status;
    snapshot.phase=stream->phase;snapshot.failure=stream->failure;
    snapshot.terminal_publication_status=stream->terminal_publication_status;
    memcpy(snapshot.completion_id,stream->completion_id,sizeof snapshot.completion_id);
    snapshot.finish_reason=stream->finish_reason;snapshot.usage=stream->usage;snapshot.has_usage=stream->has_usage;
    *out_text=text;*out_snapshot=snapshot;return UMI_STATUS_OK;
}

#endif
/* Metadata is producer-owned. This helper never runs concurrently with Feed,
 * Finish, Cancel or Fail; only the independent text-copy path may do that. */
static void AwStreamMetadata(AwLocalChatStream *stream,AwLocalChatStreamSnapshot *snapshot)
{
    snapshot->phase=stream->phase;snapshot->failure=stream->failure;
    snapshot->terminal_publication_status=stream->terminal_publication_status;
    memcpy(snapshot->completion_id,stream->completion_id,sizeof snapshot->completion_id);
    snapshot->finish_reason=stream->finish_reason;snapshot->usage=stream->usage;
    snapshot->has_usage=stream->has_usage;
}
UmiStatus AwLocalChatStreamTextCopy(AwLocalChatStream *stream,char **out_text,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiResponseStreamCopy(stream->text,out_text,out_snapshot);
}
UmiStatus AwLocalChatStreamInspect(AwLocalChatStream *stream,AwLocalChatStreamSnapshot *out)
{
    if(stream==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    AwLocalChatStreamSnapshot snapshot={0};
    UmiStatus status=UmiAiResponseStreamInspect(stream->text,&snapshot.text);
    if(status!=UMI_STATUS_OK)return status;
    AwStreamMetadata(stream,&snapshot);*out=snapshot;return UMI_STATUS_OK;
}
UmiStatus AwLocalChatStreamCopy(AwLocalChatStream *stream,char **out_text,
    AwLocalChatStreamSnapshot *out_snapshot)
{
    if(stream==NULL || out_text==NULL || out_snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    AwLocalChatStreamSnapshot snapshot={0};char *text=NULL;
    UmiStatus status=UmiAiResponseStreamCopy(stream->text,&text,&snapshot.text);
    if(status!=UMI_STATUS_OK)return status;
    AwStreamMetadata(stream,&snapshot);*out_text=text;*out_snapshot=snapshot;return UMI_STATUS_OK;
}

/* Preserve the controller ownership check and delegate byte boundaries to
 * the shared response-stream owner; do not read producer protocol metadata. */
UmiStatus AwLocalChatStreamTextCopyRange(AwLocalChatStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    if (stream == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiResponseStreamCopyRange(stream->text, offset, maximum_bytes, out_text, out_copied, out_snapshot);
}

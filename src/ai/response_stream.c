/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/response_stream.c
 *
 * PURPOSE:
 *   Serialize model stream admission and publish complete UTF-8 text without borrowed UI storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/response_stream.h"
#include "umicom/base/utf8_stream.h"
#include "umicom/platform/threading.h"
#include <stdlib.h>
#include <string.h>

struct UmiAiResponseStream {
    UmiMutex *mutex;
    unsigned char *bytes;
    UmiUtf8Stream decoder;
    UmiAiResponseStreamSnapshot snapshot;
};
static int IdentityValid(UmiAiResponseStreamIdentity identity)
{
    return identity.request_id!=0U && identity.model_revision!=0U;
}
static UmiStatus MatchIdentity(const UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity)
{
    return stream->snapshot.identity.request_id==identity.request_id &&
        stream->snapshot.identity.model_revision==identity.model_revision
        ?UMI_STATUS_OK:UMI_STATUS_NOT_FOUND;
}
static UmiStatus Admit(const UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity,uint64_t sequence)
{
    UmiStatus status=MatchIdentity(stream,identity);
    if(status!=UMI_STATUS_OK)return status;
    if(stream->snapshot.state!=UMI_AI_RESPONSE_STREAM_ACTIVE ||
        stream->snapshot.last_sequence==UINT64_MAX || sequence==0U ||
        sequence!=stream->snapshot.last_sequence+1U)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
static UmiStatus Unlock(UmiAiResponseStream *stream,UmiStatus status)
{
    UmiStatus released=umi_mutex_unlock(stream->mutex);
    return status!=UMI_STATUS_OK?status:released;
}
static void FailLocked(UmiAiResponseStream *stream,uint64_t sequence,UmiStatus status)
{
    stream->snapshot.state=UMI_AI_RESPONSE_STREAM_FAILED;
    stream->snapshot.status=status;
    stream->snapshot.terminal_sequence=sequence;
}
UmiStatus UmiAiResponseStreamCreate(UmiAiResponseStreamIdentity identity,
    size_t maximum_bytes,UmiAiResponseStream **out_stream)
{
    if(!IdentityValid(identity) || out_stream==NULL || maximum_bytes==0U ||
        maximum_bytes>UMI_AI_RESPONSE_STREAM_MAX_BYTES)return UMI_STATUS_INVALID_ARGUMENT;
    UmiAiResponseStream *stream=calloc(1U,sizeof *stream);
    if(stream==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    stream->bytes=malloc(maximum_bytes+1U);
    if(stream->bytes==NULL){free(stream);return UMI_STATUS_OUT_OF_MEMORY;}
    UmiStatus status=umi_mutex_create(&stream->mutex);
    if(status!=UMI_STATUS_OK){free(stream->bytes);free(stream);return status;}
    stream->bytes[0]=0U;
    stream->snapshot=(UmiAiResponseStreamSnapshot){
        .identity=identity,.state=UMI_AI_RESPONSE_STREAM_ACTIVE,
        .status=UMI_STATUS_OK,.maximum_bytes=maximum_bytes};
    *out_stream=stream;return UMI_STATUS_OK;
}
void UmiAiResponseStreamDestroy(UmiAiResponseStream *stream)
{
    if(stream==NULL)return;
    /* External quiescence is required: owning a mutex does not prove that no
     * producer still holds a pointer and is about to acquire it. */
    umi_mutex_destroy(stream->mutex);
    free(stream->bytes);free(stream);
}
UmiStatus UmiAiResponseStreamPush(UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity,uint64_t sequence,
    const unsigned char *bytes,size_t size,int final_chunk)
{
    if(stream==NULL || (bytes==NULL && size!=0U) ||
        (final_chunk!=0 && final_chunk!=1))return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_mutex_lock(stream->mutex);
    if(status!=UMI_STATUS_OK)return status;
    status=Admit(stream,identity,sequence);
    if(status!=UMI_STATUS_OK)return Unlock(stream,status);
    /* Reserve the last sequence for a terminal event so no accepted active
     * stream can require a wrapped sequence number. */
    if(sequence==UINT64_MAX && !final_chunk)
        return Unlock(stream,UMI_STATUS_CAPACITY_EXCEEDED);
    if(size>stream->snapshot.maximum_bytes-stream->snapshot.accepted_bytes){
        FailLocked(stream,sequence,UMI_STATUS_CAPACITY_EXCEEDED);
        return Unlock(stream,UMI_STATUS_CAPACITY_EXCEEDED);
    }
    UmiUtf8Stream candidate=stream->decoder;
    for(size_t index=0U;index<size;++index){
        unsigned char scalar[4];size_t produced=0U;
        if(bytes[index]==0U){status=UMI_STATUS_PARSE_ERROR;break;}
        status=UmiUtf8StreamAppend(&candidate,bytes[index],scalar,sizeof scalar,&produced);
        if(status!=UMI_STATUS_OK)break;
    }
    if(status==UMI_STATUS_OK && final_chunk &&
        UmiUtf8StreamFinish(&candidate)!=UMI_STATUS_OK)status=UMI_STATUS_PARSE_ERROR;
    if(status!=UMI_STATUS_OK){
        FailLocked(stream,sequence,status);
        return Unlock(stream,status);
    }
    /* Validate the whole event before exposing any of it. Only accepted raw
     * bytes are retained; copied observations stop at the complete scalar count. */
    if(size!=0U)memcpy(stream->bytes+stream->snapshot.accepted_bytes,bytes,size);
    stream->snapshot.accepted_bytes+=size;
    stream->bytes[stream->snapshot.accepted_bytes]=0U;
    stream->decoder=candidate;
    stream->snapshot.text_bytes=(size_t)candidate.completedBytes;
    stream->snapshot.pending_bytes=stream->snapshot.accepted_bytes-stream->snapshot.text_bytes;
    stream->snapshot.last_sequence=sequence;
    if(final_chunk){
        stream->snapshot.state=UMI_AI_RESPONSE_STREAM_COMPLETE;
        stream->snapshot.terminal_sequence=sequence;
    }
    return Unlock(stream,UMI_STATUS_OK);
}
UmiStatus UmiAiResponseStreamCancel(UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_mutex_lock(stream->mutex);
    if(status!=UMI_STATUS_OK)return status;
    status=MatchIdentity(stream,identity);
    if(status==UMI_STATUS_OK){
        if(stream->snapshot.state==UMI_AI_RESPONSE_STREAM_ACTIVE){
            stream->snapshot.state=UMI_AI_RESPONSE_STREAM_CANCELLED;
            stream->snapshot.status=UMI_STATUS_CANCELLED;
        }else if(stream->snapshot.state!=UMI_AI_RESPONSE_STREAM_CANCELLED)
            status=UMI_STATUS_INVALID_STATE;
    }
    return Unlock(stream,status);
}
UmiStatus UmiAiResponseStreamFail(UmiAiResponseStream *stream,
    UmiAiResponseStreamIdentity identity,uint64_t sequence,UmiStatus failure)
{
    if(stream==NULL || failure<=UMI_STATUS_OK || failure>UMI_STATUS_BUSY ||
        failure==UMI_STATUS_CANCELLED)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_mutex_lock(stream->mutex);
    if(status!=UMI_STATUS_OK)return status;
    status=Admit(stream,identity,sequence);
    if(status==UMI_STATUS_OK)FailLocked(stream,sequence,failure);
    return Unlock(stream,status);
}
UmiStatus UmiAiResponseStreamInspect(UmiAiResponseStream *stream,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    if(stream==NULL || out_snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_mutex_lock(stream->mutex);
    if(status!=UMI_STATUS_OK)return status;
    UmiAiResponseStreamSnapshot snapshot=stream->snapshot;
    status=Unlock(stream,UMI_STATUS_OK);
    if(status==UMI_STATUS_OK)*out_snapshot=snapshot;
    return status;
}
UmiStatus UmiAiResponseStreamCopy(UmiAiResponseStream *stream,
    char **out_text,UmiAiResponseStreamSnapshot *out_snapshot)
{
    if(stream==NULL || out_text==NULL || out_snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_mutex_lock(stream->mutex);
    if(status!=UMI_STATUS_OK)return status;
    UmiAiResponseStreamSnapshot snapshot=stream->snapshot;
    char *text=malloc(snapshot.text_bytes+1U);
    if(text==NULL)return Unlock(stream,UMI_STATUS_OUT_OF_MEMORY);
    memcpy(text,stream->bytes,snapshot.text_bytes);text[snapshot.text_bytes]='\0';
    status=Unlock(stream,UMI_STATUS_OK);
    if(status!=UMI_STATUS_OK){free(text);return status;}
    *out_text=text;*out_snapshot=snapshot;
    return UMI_STATUS_OK;
}

/* Copy a bounded suffix window rather than allocating the full response on
 * every observer tick. Push already proved that visible bytes are complete
 * UTF-8, so a continuation-byte boundary check is sufficient here. */
UmiStatus UmiAiResponseStreamCopyRange(UmiAiResponseStream *stream,
    size_t offset, size_t maximum_bytes, char **out_text, size_t *out_copied,
    UmiAiResponseStreamSnapshot *out_snapshot)
{
    if (stream == NULL || out_text == NULL || out_copied == NULL ||
        out_snapshot == NULL || maximum_bytes == 0U ||
        maximum_bytes > UMI_AI_RESPONSE_STREAM_MAX_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_mutex_lock(stream->mutex);
    if (status != UMI_STATUS_OK) return status;
    UmiAiResponseStreamSnapshot snapshot = stream->snapshot;
    if (offset > snapshot.text_bytes ||
        (offset < snapshot.text_bytes && (stream->bytes[offset] & 0xc0U) == 0x80U))
        return Unlock(stream, UMI_STATUS_INVALID_ARGUMENT);
    size_t remaining = snapshot.text_bytes - offset;
    size_t copied = remaining < maximum_bytes ? remaining : maximum_bytes;
    size_t end = offset + copied;
    /* Back up at most three bytes when the budget ends inside a scalar.
     * Never expose the decoder's incomplete trailing prefix. */
    while (end > offset && end < snapshot.text_bytes &&
        (stream->bytes[end] & 0xc0U) == 0x80U) --end;
    copied = end - offset;
    if (copied == 0U && remaining != 0U)
        return Unlock(stream, UMI_STATUS_CAPACITY_EXCEEDED);
    char *text = malloc(copied + 1U);
    if (text == NULL) return Unlock(stream, UMI_STATUS_OUT_OF_MEMORY);
    if (copied != 0U) memcpy(text, stream->bytes + offset, copied);
    text[copied] = '\0';
    status = Unlock(stream, UMI_STATUS_OK);
    if (status != UMI_STATUS_OK) { free(text); return status; }
    *out_text = text;
    *out_copied = copied;
    *out_snapshot = snapshot;
    return UMI_STATUS_OK;
}

void UmiAiResponseStreamTextFree(char *text)
{
    free(text);
}

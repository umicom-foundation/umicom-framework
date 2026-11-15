/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/base/utf8_stream.c
 *
 * PURPOSE:
 *   Reuse the canonical scalar decoder for incremental UTF-8 validation and publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/base/utf8_stream.h"
#include "umicom/base/utf8.h"
#include <string.h>

/* Only framing is new here; Unicode range, overlong and surrogate rules stay
 * in UmiUtf8DecodeScalar. A minimal valid completion lets that same decoder
 * reject an impossible partial prefix immediately. */
static size_t ScalarWidth(uint8_t lead)
{
    if(lead<0x80U)return 1U;
    if(lead>=0xc2U && lead<=0xdfU)return 2U;
    if(lead>=0xe0U && lead<=0xefU)return 3U;
    if(lead>=0xf0U && lead<=0xf4U)return 4U;
    return 0U;
}
static int PrefixValid(const uint8_t *bytes,size_t count,size_t width)
{
    uint8_t complete[4]={0U,0x80U,0x80U,0x80U};
    if(count==0U || count>width || width>sizeof complete)return 0;
    if(bytes[0]==0xe0U)complete[1]=0xa0U;
    else if(bytes[0]==0xf0U)complete[1]=0x90U;
    memcpy(complete,bytes,count);
    size_t used=0U;uint32_t scalar=0U;
    return UmiUtf8DecodeScalar(complete,width,&used,&scalar) && used==width;
}
static int StreamValid(const UmiUtf8Stream *stream)
{
    if(stream->pendingCount==0U)return stream->expectedBytes==0U;
    if(stream->pendingCount>=4U)return 0;
    size_t width=ScalarWidth(stream->pending[0]);
    return width>stream->pendingCount && stream->expectedBytes==width &&
        PrefixValid(stream->pending,stream->pendingCount,width);
}
UmiStatus UmiUtf8StreamReset(UmiUtf8Stream *stream)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(stream,0,sizeof *stream);
    return UMI_STATUS_OK;
}
UmiStatus UmiUtf8StreamAppend(UmiUtf8Stream *stream,uint8_t byte,
    uint8_t *output,size_t capacity,size_t *produced)
{
    if(stream==NULL || produced==NULL || (output==NULL && capacity!=0U))
        return UMI_STATUS_INVALID_ARGUMENT;
    if(!StreamValid(stream))return UMI_STATUS_INVALID_STATE;
    UmiUtf8Stream next=*stream;
    if(next.pendingCount==0U){
        next.expectedBytes=ScalarWidth(byte);
        if(next.expectedBytes==0U)return UMI_STATUS_PARSE_ERROR;
    }
    next.pending[next.pendingCount++]=byte;
    if(!PrefixValid(next.pending,next.pendingCount,next.expectedBytes))
        return UMI_STATUS_PARSE_ERROR;
    if(next.pendingCount<next.expectedBytes){
        *stream=next;*produced=0U;return UMI_STATUS_OK;
    }
    size_t count=next.expectedBytes;
    if(capacity<count)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(next.completedScalars==UINT64_MAX || next.completedBytes>UINT64_MAX-count)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    ++next.completedScalars;next.completedBytes+=(uint64_t)count;
    next.pendingCount=0U;next.expectedBytes=0U;
    memcpy(output,next.pending,count);
    memset(next.pending,0,sizeof next.pending);
    *stream=next;*produced=count;
    return UMI_STATUS_OK;
}
UmiStatus UmiUtf8StreamFinish(const UmiUtf8Stream *stream)
{
    if(stream==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(!StreamValid(stream) || stream->pendingCount!=0U)return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
UmiStatus UmiUtf8StreamInspect(const UmiUtf8Stream *stream,UmiUtf8StreamInfo *out_info)
{
    if(stream==NULL || out_info==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(!StreamValid(stream))return UMI_STATUS_INVALID_STATE;
    UmiUtf8StreamInfo info={stream->completedScalars,stream->completedBytes,
        stream->expectedBytes-stream->pendingCount};
    *out_info=info;return UMI_STATUS_OK;
}

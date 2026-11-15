/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/stream.c
 *
 * PURPOSE:
 *   Append incremental model-output chunks and notify an optional sink.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The collector separates transport chunking from application state so GTK4, CLI and headless tests can consume the same stream.
 */

#include "umicom/ai/stream.h"
#include <stddef.h>

#include <string.h>

/*
 * Initialise ai stream from caller-provided values so later operations receive a known
 * state.
 */
void umi_ai_stream_init(UmiAiStreamCollector *stream,
                        UmiAiStreamSink sink,
                        void *user_data)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream != NULL) {
        stream->text[0] = '\0';
        stream->length = 0U;
        stream->sink = sink;
        stream->user_data = user_data;
    }
}

/* Provide the ai stream push operation used by this module and its client applications. */
/* Bound compatibility input and snapshot overlapping chunks before appending. The previous unchecked strlen/addition/memcpy implementation remains for review; the public struct and callback convention are preserved. The previous implementation remains for review. */
#if 0
UmiStatus umi_ai_stream_push(UmiAiStreamCollector *stream,
                             const char *text,
                             int final_chunk)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (stream->length + length + 1U > sizeof(stream->text)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(stream->text + stream->length, text, length + 1U);
    stream->length += length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream->sink != NULL) {
        stream->sink(text, final_chunk, stream->user_data);
    }
    return UMI_STATUS_OK;
}
#endif
UmiStatus umi_ai_stream_push(UmiAiStreamCollector *stream,
    const char *text,int final_chunk)
{
    if(stream==NULL || text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* The public compatibility struct is writable. Validate its stored length
     * before subtraction or indexing, then bound the input by available space. */
    if(stream->length>=sizeof stream->text || stream->text[stream->length]!='\0' ||
        memchr(stream->text,'\0',stream->length)!=NULL)return UMI_STATUS_INVALID_STATE;
    size_t available=sizeof stream->text-stream->length;
    size_t length=0U;
    while(length<available && text[length]!='\0')++length;
    if(length==available)return UMI_STATUS_CAPACITY_EXCEEDED;
    /* A fixed owned chunk supports self-append and overlapping source slices.
     * The sink borrows this stable chunk only during the call, including when
     * it resets or appends to the collector. No owner access follows the sink. */
    char chunk[sizeof stream->text];
    memcpy(chunk,text,length+1U);
    memcpy(stream->text+stream->length,chunk,length+1U);
    stream->length+=length;
    UmiAiStreamSink sink=stream->sink;void *user_data=stream->user_data;
    if(sink!=NULL)sink(chunk,final_chunk,user_data);
    return UMI_STATUS_OK;
}

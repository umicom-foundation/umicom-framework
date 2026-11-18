/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/jsonl_stream.c
 *
 * PURPOSE:
 *   Compose canonical file chunks and strict pair parsing without retaining the full dataset.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/training/jsonl_stream.h"
#include "umicom/platform/input_stream.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>

typedef struct StreamContext {
    unsigned char *line;
    size_t used,received;
    const UmiCancellationToken *cancel;
    UmiAiTrainingPairVisitor visitor;
    void *user;
    UmiAiTrainingJsonlReport *report;
    int callbackFailed;
} StreamContext;

static UmiStatus AdmitRecord(StreamContext *stream)
{
    stream->report->stage=UMI_AI_TRAINING_JSONL_RECORD;
    stream->report->failedLine=stream->report->acceptedRows+1U;
    if(umi_cancellation_token_is_requested(stream->cancel)) return UMI_STATUS_CANCELLED;
    if(stream->report->acceptedRows==UMI_AI_TRAINING_JSONL_ROWS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
static UmiStatus FinishRecord(StreamContext *stream)
{
    size_t bytes=stream->used;
    if(bytes!=0U && stream->line[bytes-1U]=='\r') --bytes;
    if(bytes==0U) return UMI_STATUS_PARSE_ERROR;
    UmiAiTrainingPair pair;
    /* One canonical parser owns keys, Unicode, escapes and field bounds.
     * Streaming only assembles physical lines; it never interprets JSON. */
    UmiStatus status=UmiAiTrainingPairRead(stream->line,bytes,stream->cancel,&pair);
    if(status==UMI_STATUS_OK) {
        stream->report->stage=UMI_AI_TRAINING_JSONL_VISITOR;
        status=stream->visitor(stream->user,&pair,stream->report->failedLine);
    }
    umi_secret_clear(&pair,sizeof(pair));
    if(status==UMI_STATUS_OK) {
        ++stream->report->acceptedRows;
        stream->report->consumedBytes=stream->received;
        umi_secret_clear(stream->line,stream->used);
        stream->used=0U;
    }
    return status;
}
static UmiStatus ConsumeChunk(const unsigned char *bytes,size_t size,void *opaque)
{
    StreamContext *stream=opaque;
    UmiStatus status=UMI_STATUS_OK;
    if(umi_cancellation_token_is_requested(stream->cancel)) {
        status=AdmitRecord(stream);
        goto failed;
    }
    for(size_t i=0U;i<size;++i) {
        if(stream->used==0U) {
            status=AdmitRecord(stream);
            if(status!=UMI_STATUS_OK) goto failed;
        }
        ++stream->received; /* The platform admitted at most JSONL_BYTES. */
        if(bytes[i]=='\n') {
            status=FinishRecord(stream);
            if(status!=UMI_STATUS_OK) goto failed;
        } else {
            /* Permit exactly one extra CR while awaiting LF or EOF. It is a
             * delimiter candidate, not permission to enlarge the JSON record. */
            if(stream->used>UMI_AI_TRAINING_PAIR_LINE_BYTES ||
                (stream->used==UMI_AI_TRAINING_PAIR_LINE_BYTES && bytes[i]!='\r')) {
                status=UMI_STATUS_CAPACITY_EXCEEDED;
                goto failed;
            }
            stream->line[stream->used++]=bytes[i];
        }
    }
    return UMI_STATUS_OK;
failed:
    stream->callbackFailed=1;
    return status;
}
UmiStatus UmiAiTrainingJsonlVisitFile(const char *absolutePath,
    const UmiCancellationToken *cancel,UmiAiTrainingPairVisitor visitor,
    void *context,UmiAiTrainingJsonlReport *outReport)
{
    if(outReport==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outReport=(UmiAiTrainingJsonlReport){0};
    if(absolutePath==NULL || visitor==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    const size_t capacity=UMI_AI_TRAINING_PAIR_LINE_BYTES+1U;
    unsigned char *line=malloc(capacity);
    if(line==NULL) return UMI_STATUS_OUT_OF_MEMORY;
    StreamContext stream={.line=line,.cancel=cancel,.visitor=visitor,
        .user=context,.report=outReport};
    uint64_t fileBytes=0U;
    UmiStatus status=UmiInputFileVisit(absolutePath,UMI_AI_TRAINING_JSONL_BYTES,
        ConsumeChunk,&stream,&fileBytes);
    if(status!=UMI_STATUS_OK && !stream.callbackFailed) {
        /* Accepted callbacks can precede a later file identity/close error.
         * Preserve that progress but never label the overall file complete. */
        outReport->stage=UMI_AI_TRAINING_JSONL_INPUT;
        outReport->failedLine=0U;
    }
    if(status==UMI_STATUS_OK && fileBytes==0U) status=UMI_STATUS_INVALID_ARGUMENT;
    if(status==UMI_STATUS_OK && stream.used!=0U) status=FinishRecord(&stream);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) {
        outReport->stage=UMI_AI_TRAINING_JSONL_INPUT;
        outReport->failedLine=0U;
        status=UMI_STATUS_CANCELLED;
    }
    if(status==UMI_STATUS_OK) {
        outReport->stage=UMI_AI_TRAINING_JSONL_COMPLETE;
        outReport->failedLine=0U;
    }
    umi_secret_clear(line,capacity);
    free(line);
    return status;
}

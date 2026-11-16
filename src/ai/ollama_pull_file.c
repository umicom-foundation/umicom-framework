/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/ollama_pull_file.c
 *
 * PURPOSE:
 *   Compose regular-file visitation, native progress framing and SHA-256 before publishing a complete observation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_pull_file.h"
#include "umicom/platform/input_stream.h"
#include "umicom/security/secrets.h"
#include "umicom/protocol/json_writer.h"
#include <string.h>

typedef struct PullFileContext {
    UmiOllamaPullStream *stream;
    UmiSha256 digest;
    const UmiCancellationToken *cancel;
} PullFileContext;
static UmiStatus PullFileChunk(const unsigned char *bytes,size_t length,void *context)
{
    PullFileContext *owner=context;
    UmiStatus status=UmiOllamaPullStreamFeed(owner->stream,bytes,length,owner->cancel);
    return status==UMI_STATUS_OK?UmiSha256Update(&owner->digest,bytes,length):status;
}
UmiStatus UmiOllamaPullFileInspect(const char *path,const UmiCancellationToken *cancel,
    UmiOllamaPullFileReport *out)
{
    if(path==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
    PullFileContext context={0};context.cancel=cancel;UmiSha256Init(&context.digest);
    UmiOllamaPullFileReport report={0};UmiOllamaPullSnapshot view={0};
    UmiStatus status=UmiOllamaPullStreamCreate(NULL,NULL,&context.stream);
    if(status==UMI_STATUS_OK)status=UmiInputFileVisit(path,UMI_OLLAMA_PULL_STREAM_BYTES,
        PullFileChunk,&context,&report.bytes);
    if(status==UMI_STATUS_OK){
        /* Only a fully read and successfully closed file may be sealed. A
         * provider error is a complete observation, distinct from file I/O. */
        UmiStatus finished=UmiOllamaPullStreamFinish(context.stream,cancel);
        status=UmiOllamaPullStreamRead(context.stream,&view);
        if(status==UMI_STATUS_OK){
            if(finished==UMI_STATUS_OK || (finished==UMI_STATUS_IO_ERROR && view.input_finished &&
                view.terminal_seen && view.last.kind==UMI_OLLAMA_PULL_ERROR)){
                report.events=view.events;report.provider_outcome=view.last.kind;
                status=UmiSha256Final(&context.digest,report.file_sha256);
            }else status=finished;
        }
    }
    UmiStatus release=UmiOllamaPullStreamRelease(&context.stream);
    if(status==UMI_STATUS_OK)status=release;
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK)*out=report;
    umi_secret_clear(&view,sizeof view);umi_secret_clear(&context,sizeof context);
    umi_secret_clear(&report,sizeof report);return status;
}
UmiStatus UmiOllamaPullFileJson(const UmiOllamaPullFileReport *report,
    char *out,size_t capacity,size_t *out_length)
{
    if(report==NULL || out==NULL || out_length==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(report->bytes==0U || report->bytes>UMI_OLLAMA_PULL_STREAM_BYTES ||
       report->events==0U || report->events>UMI_OLLAMA_PULL_MAX_EVENTS ||
       (report->provider_outcome!=UMI_OLLAMA_PULL_SUCCESS && report->provider_outcome!=UMI_OLLAMA_PULL_ERROR))
        return UMI_STATUS_INVALID_ARGUMENT;
    char staged[UMI_OLLAMA_PULL_FILE_JSON_CAPACITY],digest[UMI_SHA256_HEX_CAPACITY];
    UmiSha256Hex(report->file_sha256,digest);UmiJsonWriter writer;
    UmiJsonWriterInit(&writer,staged,sizeof staged);
    UmiJsonWriterRaw(&writer,"{\"schema_version\":1,\"scope\":\"offline-pull-progress-file\",\"bytes\":");
    UmiJsonWriterUInt64(&writer,report->bytes);
    UmiJsonWriterRaw(&writer,",\"events\":");UmiJsonWriterUInt64(&writer,(uint64_t)report->events);
    UmiJsonWriterRaw(&writer,",\"file_sha256\":");UmiJsonWriterString(&writer,digest);
    UmiJsonWriterRaw(&writer,",\"provider_outcome\":");
    UmiJsonWriterString(&writer,report->provider_outcome==UMI_OLLAMA_PULL_SUCCESS?"success":"error");
    UmiJsonWriterRaw(&writer,",\"server_identity_verified\":false,\"model_installation_verified\":false}");
    if(writer.status!=UMI_STATUS_OK)return writer.status;
    if(capacity<=writer.length)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out,staged,writer.length+1U);*out_length=writer.length;return UMI_STATUS_OK;
}

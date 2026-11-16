/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_queue_json.c
 *
 * PURPOSE:
 *   Publish escaped queue metadata pages atomically while preserving exact scheduling tokens and observed identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_queue_file.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/security/secrets.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void QueueText(UmiJsonWriter *writer,const char *key,const char *text)
{
    UmiJsonWriterRaw(writer,",");UmiJsonWriterString(writer,key);
    UmiJsonWriterRaw(writer,":");UmiJsonWriterString(writer,text);
}
static void QueueCount(UmiJsonWriter *writer,const char *key,size_t value)
{
    UmiJsonWriterRaw(writer,",");UmiJsonWriterString(writer,key);
    UmiJsonWriterRaw(writer,":");UmiJsonWriterUInt64(writer,(uint64_t)value);
}
static void QueueDigest(UmiJsonWriter *writer,const char *key,const unsigned char *value)
{
    char hex[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(value,hex);QueueText(writer,key,hex);
}
UmiStatus UmiComfyQueueJson(const UmiComfyQueue *queue,size_t offset,size_t limit,
    char *out,size_t capacity,size_t *out_length)
{
    if(out==NULL || out_length==NULL || limit==0U || limit>UMI_COMFY_QUEUE_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(capacity==0U)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiComfyQueueReview review;
    UmiStatus status=UmiComfyQueueInspect(queue,&review);if(status!=UMI_STATUS_OK)return status;
    size_t total=UmiComfyQueueCount(queue);if(offset>total)return UMI_STATUS_NOT_FOUND;
    size_t count=total-offset;if(count>limit)count=limit;
    size_t bound=capacity<UMI_COMFY_QUEUE_JSON_CAPACITY?capacity:UMI_COMFY_QUEUE_JSON_CAPACITY;
    char *staged=malloc(bound);if(staged==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;UmiJsonWriterInit(&writer,staged,bound);
    UmiJsonWriterRaw(&writer,"{\"schema_version\":1,\"scope\":\"queue-observations\"");
    char revision[32];int length=snprintf(revision,sizeof revision,"%" PRIu64,review.connection_revision);
    if(length<0 || (size_t)length>=sizeof revision){status=UMI_STATUS_INVALID_STATE;goto done;}
    QueueText(&writer,"connection_revision",revision);
    QueueDigest(&writer,"connection_sha256",review.connection_sha256);
    QueueDigest(&writer,"response_sha256",review.response_sha256);
    QueueCount(&writer,"response_bytes",review.response_bytes);
    QueueCount(&writer,"running_count",review.running_count);
    QueueCount(&writer,"pending_count",review.pending_count);
    QueueCount(&writer,"entry_count",total);
    QueueCount(&writer,"offset",offset);QueueCount(&writer,"page_count",count);
    QueueCount(&writer,"remaining_count",total-offset-count);
    QueueCount(&writer,"next_offset",offset+count);
    UmiJsonWriterRaw(&writer,",\"entries\":[");
    for(size_t i=0U;i<count;++i){
        UmiComfyQueueEntry entry;
        status=UmiComfyQueueAt(queue,offset+i,&entry);if(status!=UMI_STATUS_OK)goto done;
        if(i!=0U)UmiJsonWriterRaw(&writer,",");
        UmiJsonWriterRaw(&writer,"{\"prompt_id\":");UmiJsonWriterString(&writer,entry.prompt_id);
        QueueText(&writer,"number_json",entry.number_json);
        QueueText(&writer,"state",entry.state==UMI_COMFY_QUEUE_RUNNING?"running":"pending");
        QueueCount(&writer,"response_index",offset+i);
        UmiJsonWriterRaw(&writer,"}");
        umi_secret_clear(&entry,sizeof entry);
    }
    UmiJsonWriterRaw(&writer,"]}");status=writer.status;
    if(status==UMI_STATUS_OK){memcpy(out,staged,writer.length+1U);*out_length=writer.length;}
done:
    umi_secret_clear(staged,bound);free(staged);return status;
}

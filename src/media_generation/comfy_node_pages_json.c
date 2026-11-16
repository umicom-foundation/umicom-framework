/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/media_generation/comfy_node_pages_json.c
 *
 * PURPOSE:
 *   Stage bounded node pages with stable ordered-selection identity and explicit unknown flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/media_generation/comfy_node_pages.h"
#include "umicom/protocol/json_writer.h"
#include "umicom/security/secrets.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void NodeKey(UmiJsonWriter *writer,const char *key)
{
    UmiJsonWriterRaw(writer,",");UmiJsonWriterString(writer,key);UmiJsonWriterRaw(writer,":");
}
static void NodeText(UmiJsonWriter *writer,const char *key,const char *text)
{
    NodeKey(writer,key);UmiJsonWriterString(writer,text);
}
static void NodeCount(UmiJsonWriter *writer,const char *key,size_t value)
{
    NodeKey(writer,key);UmiJsonWriterUInt64(writer,(uint64_t)value);
}
static void NodeDigest(UmiJsonWriter *writer,const char *key,const unsigned char *digest)
{
    char hex[UMI_SHA256_HEX_CAPACITY];UmiSha256Hex(digest,hex);NodeText(writer,key,hex);
}
static void NodeFlag(UmiJsonWriter *writer,const char *key,const UmiComfyNodeInfo *info,unsigned bit)
{
    NodeKey(writer,key);
    if(info->known_flags&bit)UmiJsonWriterBool(writer,(info->enabled_flags&bit)!=0U);
    else UmiJsonWriterRaw(writer,"null");
}
static UmiStatus NodeSelectionDigest(const UmiComfyNodeSelection *selection,size_t count,
    unsigned char digest[UMI_SHA256_BYTES])
{
    /* Class IDs are unique, contain no decoded NUL and are bounded by the
     * catalogue. Terminators frame them unambiguously; catalogue identity binds
     * their metadata. Hash the complete order, not only this requested page. */
    static const char domain[]="Umicom.ComfySelection.v1";
    UmiSha256 hash;UmiSha256Init(&hash);
    UmiStatus status=UmiSha256Update(&hash,domain,sizeof domain);
    for(size_t i=0U;status==UMI_STATUS_OK && i<count;++i){
        UmiComfyNodeInfo info;status=UmiComfyNodeSelectionAt(selection,i,&info);
        if(status==UMI_STATUS_OK)status=UmiSha256Update(&hash,info.class_type,strlen(info.class_type)+1U);
        umi_secret_clear(&info,sizeof info);
    }
    if(status==UMI_STATUS_OK)status=UmiSha256Final(&hash,digest);
    umi_secret_clear(&hash,sizeof hash);return status;
}
UmiStatus UmiComfyNodeSelectionJson(const UmiComfyNodeSelection *selection,size_t offset,
    size_t limit,char *out,size_t capacity,size_t *out_length)
{
    if(out==NULL || out_length==NULL || limit==0U || limit>UMI_COMFY_NODE_PAGE_LIMIT)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(capacity==0U)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiComfyNodeSelectionReview review;
    UmiStatus status=UmiComfyNodeSelectionInspect(selection,&review);
    if(status!=UMI_STATUS_OK)return status;
    if(offset>review.selected_count)return UMI_STATUS_NOT_FOUND;
    size_t count=review.selected_count-offset;if(count>limit)count=limit;
    unsigned char digest[UMI_SHA256_BYTES];status=NodeSelectionDigest(selection,review.selected_count,digest);
    if(status!=UMI_STATUS_OK)return status;
    size_t bound=capacity<UMI_COMFY_NODE_PAGE_JSON_CAPACITY?capacity:UMI_COMFY_NODE_PAGE_JSON_CAPACITY;
    char *staged=malloc(bound);if(staged==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    UmiJsonWriter writer;UmiJsonWriterInit(&writer,staged,bound);
    UmiJsonWriterRaw(&writer,"{\"schema_version\":1,\"scope\":\"node-selection-observations\"");
    char revision[32];int length=snprintf(revision,sizeof revision,"%" PRIu64,review.catalogue.connection_revision);
    if(length<0 || (size_t)length>=sizeof revision){status=UMI_STATUS_INVALID_STATE;goto done;}
    NodeText(&writer,"connection_revision",revision);
    NodeDigest(&writer,"connection_sha256",review.catalogue.connection_sha256);
    NodeDigest(&writer,"response_sha256",review.catalogue.response_sha256);
    NodeDigest(&writer,"selection_sha256",digest);
    NodeCount(&writer,"source_count",review.source_count);NodeCount(&writer,"selected_count",review.selected_count);
    NodeCount(&writer,"offset",offset);NodeCount(&writer,"page_count",count);
    NodeCount(&writer,"next_offset",offset+count);NodeCount(&writer,"remaining_count",review.selected_count-offset-count);
    UmiJsonWriterRaw(&writer,",\"nodes\":[");
    for(size_t i=0U;i<count;++i){
        UmiComfyNodeInfo info;status=UmiComfyNodeSelectionAt(selection,offset+i,&info);
        if(status!=UMI_STATUS_OK)goto done;
        if(i!=0U)UmiJsonWriterRaw(&writer,",");
        UmiJsonWriterRaw(&writer,"{\"class_type\":");UmiJsonWriterString(&writer,info.class_type);
        NodeText(&writer,"display_name",info.display_name);NodeText(&writer,"category",info.category);
        NodeCount(&writer,"selection_index",offset+i);
        NodeCount(&writer,"required_count",info.required_count);NodeCount(&writer,"optional_count",info.optional_count);
        NodeCount(&writer,"output_count",info.output_count);
        NodeCount(&writer,"known_flags",info.known_flags);NodeCount(&writer,"enabled_flags",info.enabled_flags);
        NodeFlag(&writer,"output_node",&info,UMI_COMFY_NODE_OUTPUT_NODE);
        NodeFlag(&writer,"deprecated",&info,UMI_COMFY_NODE_DEPRECATED);
        NodeFlag(&writer,"experimental",&info,UMI_COMFY_NODE_EXPERIMENTAL);
        NodeFlag(&writer,"api_node",&info,UMI_COMFY_NODE_API_NODE);
        UmiJsonWriterRaw(&writer,"}");umi_secret_clear(&info,sizeof info);
    }
    UmiJsonWriterRaw(&writer,"]}");status=writer.status;
    /* Publish once; short buffers never expose a partially formatted report. */
    if(status==UMI_STATUS_OK){memcpy(out,staged,writer.length+1U);*out_length=writer.length;}
done:
    umi_secret_clear(staged,bound);free(staged);return status;
}

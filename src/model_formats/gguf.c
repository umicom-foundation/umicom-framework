/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/model_formats/gguf.c
 *
 * PURPOSE:
 *   Parse bounded GGUF metadata incrementally with fixed storage and explicit auxiliary-role observations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "gguf_internal.h"
#include <string.h>

static uint64_t little(const unsigned char *bytes,size_t count)
{
    uint64_t result=0U;
    for(size_t i=0U;i<count;++i) result|=(uint64_t)bytes[i]<<(8U*i);
    return result;
}
static void phase(GgufScan *scan,GgufPhase next,uint64_t bytes)
{
    scan->phase=next;scan->remaining=bytes;scan->used=0U;
}
void GgufScanInit(GgufScan *scan,const UmiCancellationToken *cancel)
{
    memset(scan,0,sizeof(*scan));scan->cancel=cancel;
    phase(scan,GGUF_HEADER,24U);
}
static UmiStatus begin_value(GgufScan *scan,uint32_t type)
{
    if(type>12U) return UMI_STATUS_PARSE_ERROR;
    if(scan->values==UMI_GGUF_MAX_VALUES) return UMI_STATUS_CAPACITY_EXCEEDED;
    ++scan->values;scan->type=type;
    if(type==8U) phase(scan,GGUF_STRING_LENGTH,8U);
    else if(type==9U) phase(scan,GGUF_ARRAY_TYPE,4U);
    else {
        static const unsigned char widths[]={1U,1U,2U,2U,4U,4U,4U,1U,0U,0U,8U,8U,8U};
        phase(scan,GGUF_SCALAR,widths[type]);
    }
    return UMI_STATUS_OK;
}
static UmiStatus value_done(GgufScan *scan)
{
    /* Pop bounded array frames iteratively. Large or nested values never grow
     * the C call stack or allocate a metadata-sized object. */
    while(scan->depth!=0U) {
        GgufArray *array=&scan->arrays[scan->depth-1U];
        --array->remaining;
        if(array->remaining!=0U) return begin_value(scan,array->type);
        --scan->depth;
    }
    ++scan->entries_done;
    scan->selected=0U;
    if(scan->entries_done==scan->metadata.entries) phase(scan,GGUF_DONE,0U);
    else phase(scan,GGUF_KEY_LENGTH,8U);
    return UMI_STATUS_OK;
}
static char *selected_text(GgufScan *scan)
{
    return scan->selected==1U?scan->metadata.architecture:
        scan->selected==2U?scan->metadata.type:scan->metadata.adapter_type;
}
static UmiStatus selected_key(GgufScan *scan)
{
    scan->selected=strcmp(scan->key,"general.architecture")==0?1U:
        strcmp(scan->key,"general.type")==0?2U:strcmp(scan->key,"adapter.type")==0?3U:0U;
    if(scan->selected!=0U) {
        bool *seen=scan->selected==1U?&scan->metadata.has_architecture:
            scan->selected==2U?&scan->metadata.has_type:&scan->metadata.has_adapter_type;
        if(*seen) return UMI_STATUS_PARSE_ERROR;
        *seen=true;
    }
    phase(scan,GGUF_VALUE_TYPE,4U);
    return UMI_STATUS_OK;
}
static UmiStatus advance(GgufScan *scan)
{
    uint64_t value;
    switch(scan->phase) {
    case GGUF_HEADER:
        if(memcmp(scan->field,"GGUF",4U)!=0) return UMI_STATUS_PARSE_ERROR;
        scan->metadata.version=(uint32_t)little(scan->field+4U,4U);
        if(scan->metadata.version!=2U && scan->metadata.version!=3U) return UMI_STATUS_NOT_IMPLEMENTED;
        scan->metadata.tensors=little(scan->field+8U,8U);
        scan->metadata.entries=little(scan->field+16U,8U);
        if(scan->metadata.entries>UMI_GGUF_MAX_METADATA_ENTRIES) return UMI_STATUS_CAPACITY_EXCEEDED;
        phase(scan,scan->metadata.entries==0U?GGUF_DONE:GGUF_KEY_LENGTH,scan->metadata.entries==0U?0U:8U);
        return UMI_STATUS_OK;
    case GGUF_KEY_LENGTH:
        value=little(scan->field,8U);
        if(value==0U) return UMI_STATUS_PARSE_ERROR;
        if(value>=sizeof(scan->key)) return UMI_STATUS_CAPACITY_EXCEEDED;
        memset(scan->key,0,sizeof(scan->key));phase(scan,GGUF_KEY,value);
        return UMI_STATUS_OK;
    case GGUF_KEY: return selected_key(scan);
    case GGUF_VALUE_TYPE:
        value=little(scan->field,4U);
        if(scan->selected!=0U && value!=8U) return UMI_STATUS_PARSE_ERROR;
        return begin_value(scan,(uint32_t)value);
    case GGUF_SCALAR:
        if(scan->type==7U && scan->field[0]>1U) return UMI_STATUS_PARSE_ERROR;
        return value_done(scan);
    case GGUF_STRING_LENGTH:
        value=little(scan->field,8U);
        if(value>UMI_GGUF_MAX_METADATA_BYTES-scan->consumed) return UMI_STATUS_CAPACITY_EXCEEDED;
        if(scan->selected!=0U && (value==0U || value>=64U))
            return value==0U?UMI_STATUS_PARSE_ERROR:UMI_STATUS_CAPACITY_EXCEEDED;
        if(value==0U) return value_done(scan);
        phase(scan,GGUF_STRING,value);return UMI_STATUS_OK;
    case GGUF_STRING: return value_done(scan);
    case GGUF_ARRAY_TYPE:
        value=little(scan->field,4U);
        if(value>12U) return UMI_STATUS_PARSE_ERROR;
        scan->array_type=(uint32_t)value;phase(scan,GGUF_ARRAY_LENGTH,8U);return UMI_STATUS_OK;
    case GGUF_ARRAY_LENGTH:
        value=little(scan->field,8U);
        if(value>UMI_GGUF_MAX_VALUES-scan->values) return UMI_STATUS_CAPACITY_EXCEEDED;
        if(value==0U) return value_done(scan);
        if(scan->depth==UMI_GGUF_MAX_ARRAY_DEPTH) return UMI_STATUS_CAPACITY_EXCEEDED;
        scan->arrays[scan->depth++]=(GgufArray){scan->array_type,value};
        return begin_value(scan,scan->array_type);
    case GGUF_DONE: return UMI_STATUS_OK;
    }
    return UMI_STATUS_INVALID_STATE;
}
UmiStatus GgufScanFeed(GgufScan *scan,const unsigned char *bytes,size_t size)
{
    if(bytes==NULL && size!=0U) return UMI_STATUS_INVALID_ARGUMENT;
    size_t offset=0U;
    while(offset<size && scan->phase!=GGUF_DONE) {
        if(umi_cancellation_token_is_requested(scan->cancel)) return UMI_STATUS_CANCELLED;
        size_t take=size-offset;
        if((uint64_t)take>scan->remaining) take=(size_t)scan->remaining;
        if((uint64_t)take>UMI_GGUF_MAX_METADATA_BYTES-scan->consumed)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        const unsigned char *part=bytes+offset;
        if(scan->phase==GGUF_KEY || (scan->phase==GGUF_STRING && scan->selected!=0U)) {
            for(size_t i=0U;i<take;++i) if(part[i]<0x21U || part[i]>0x7eU) return UMI_STATUS_PARSE_ERROR;
            char *destination=scan->phase==GGUF_KEY?scan->key:selected_text(scan);
            if(take!=0U) memcpy(destination+scan->used,part,take);
            scan->used+=take;
        } else if(scan->phase!=GGUF_STRING) {
            if(take!=0U) memcpy(scan->field+scan->used,part,take);
            scan->used+=take;
        }
        /* Unselected strings are skipped directly in the caller's chunk. This
         * avoids a second copy of large tokenizer vocabularies or templates. */
        scan->remaining-=take;scan->consumed+=take;offset+=take;
        if(scan->remaining==0U) {
            UmiStatus status=advance(scan);
            if(status!=UMI_STATUS_OK) return status;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus GgufScanFinish(GgufScan *scan,UmiGgufMetadata *out)
{
    if(umi_cancellation_token_is_requested(scan->cancel)) return UMI_STATUS_CANCELLED;
    if(scan->phase!=GGUF_DONE) return UMI_STATUS_PARSE_ERROR;
    UmiGgufMetadata *metadata=&scan->metadata;
    metadata->metadata_bytes=scan->consumed;
    if(strcmp(metadata->architecture,"controlvector")==0 || strcmp(metadata->architecture,"clip")==0 ||
       strcmp(metadata->type,"adapter")==0 || metadata->has_adapter_type)
        metadata->role=UMI_GGUF_ROLE_AUXILIARY;
    else if(metadata->tensors==0U) metadata->role=UMI_GGUF_ROLE_VOCABULARY_ONLY;
    else if(metadata->has_architecture && (!metadata->has_type || strcmp(metadata->type,"model")==0))
        metadata->role=UMI_GGUF_ROLE_MODEL_CANDIDATE;
    else metadata->role=UMI_GGUF_ROLE_UNKNOWN;
    *out=*metadata;
    return UMI_STATUS_OK;
}
UmiStatus UmiGgufMetadataInspect(const unsigned char *prefix,size_t prefix_bytes,
    const UmiCancellationToken *cancel,UmiGgufMetadata *out_metadata)
{
    if(prefix==NULL || out_metadata==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    GgufScan scan;GgufScanInit(&scan,cancel);
    UmiStatus status=GgufScanFeed(&scan,prefix,prefix_bytes);
    if(status==UMI_STATUS_OK) status=GgufScanFinish(&scan,out_metadata);
    return status;
}

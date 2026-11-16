/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/model_formats/safetensors.c
 *
 * PURPOSE:
 *   Admit native tensor layout metadata using shared bounded JSON, uniqueness and cancellation services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/model_formats/safetensors.h"
#include "umicom/language_runtime/json_tree.h"
#include "header_internal.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct TensorRange { uint64_t begin, end; } TensorRange;
static int tensor_range_order(const void *left,const void *right)
{
    const TensorRange *a=left,*b=right;
    if(a->begin!=b->begin) return a->begin<b->begin?-1:1;
    return a->end<b->end?-1:a->end>b->end?1:0;
}
/* The shared JSON reader has validated grammar. Read unsigned integer spelling
 * directly from its borrowed span to avoid signed or floating-point coercion. */
static UmiStatus tensor_unsigned(const UmiJsonTree *tree,int node,uint64_t *out)
{
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_PRIMITIVE) return UMI_STATUS_PARSE_ERROR;
    const char *text=NULL;size_t length=0U;
    UmiStatus status=UmiJsonTreeSourceSpan(tree,node,&text,&length);
    if(status!=UMI_STATUS_OK) return status;
    uint64_t number=0U;
    if(length==0U) return UMI_STATUS_PARSE_ERROR;
    for(size_t i=0U;i<length;++i) {
        if(text[i]<'0' || text[i]>'9') return UMI_STATUS_PARSE_ERROR;
        unsigned digit=(unsigned)(text[i]-'0');
        if(number>(UINT64_MAX-digit)/10U) return UMI_STATUS_CAPACITY_EXCEEDED;
        number=number*10U+digit;
    }
    *out=number;return UMI_STATUS_OK;
}
static UmiStatus tensor_width(const UmiJsonTree *tree,int node,uint64_t *width)
{
    static const struct { const char *name; uint64_t bytes; } types[]={
        {"BOOL",1U},{"U8",1U},{"I8",1U},{"U16",2U},{"I16",2U},
        {"U32",4U},{"I32",4U},{"U64",8U},{"I64",8U},{"F16",2U},
        {"BF16",2U},{"F32",4U},{"F64",8U},{"C64",8U},
        {"F8_E5M2",1U},{"F8_E4M3",1U},{"F8_E8M0",1U},
        {"F8_E4M3FNUZ",1U},{"F8_E5M2FNUZ",1U}};
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
    char name[64];UmiStatus status=UmiJsonTreeText(tree,node,name,sizeof(name));
    if(status==UMI_STATUS_CAPACITY_EXCEEDED) return UMI_STATUS_NOT_IMPLEMENTED;
    if(status!=UMI_STATUS_OK) return status;
    for(size_t i=0U;i<sizeof(types)/sizeof(types[0]);++i)
        if(strcmp(name,types[i].name)==0) { *width=types[i].bytes;return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_IMPLEMENTED;
}
static UmiStatus tensor_record(const UmiJsonTree *tree,int node,uint64_t data_bytes,
    TensorRange *range,uint64_t *elements,size_t *rank)
{
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    size_t fields=UmiJsonTreeCount(tree,node);
    if(fields<3U) return UMI_STATUS_PARSE_ERROR;
    if(fields>3U) return UMI_STATUS_NOT_IMPLEMENTED;
    int dtype,shape,offsets;
    const char *names[]={"dtype","shape","data_offsets"};
    int *nodes[]={&dtype,&shape,&offsets};
    for(size_t i=0U;i<3U;++i) {
        UmiStatus found=UmiJsonTreeMember(tree,node,names[i],nodes[i]);
        if(found!=UMI_STATUS_OK) return found==UMI_STATUS_NOT_FOUND?UMI_STATUS_PARSE_ERROR:found;
    }
    uint64_t width;
    UmiStatus status=tensor_width(tree,dtype,&width);
    if(status!=UMI_STATUS_OK) return status;
    if(UmiJsonTreeKind(tree,shape)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
        UmiJsonTreeKind(tree,offsets)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY ||
        UmiJsonTreeCount(tree,offsets)!=2U) return UMI_STATUS_PARSE_ERROR;
    *rank=UmiJsonTreeCount(tree,shape);
    if(*rank>UMI_SAFETENSORS_MAX_RANK) return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t dimensions[UMI_SAFETENSORS_MAX_RANK];bool empty=false;
    int dimension=UmiJsonTreeFirst(tree,shape);
    for(size_t i=0U;i<*rank;++i) {
        status=tensor_unsigned(tree,dimension,&dimensions[i]);
        if(status!=UMI_STATUS_OK) return status;
        empty=empty || dimensions[i]==0U;dimension=UmiJsonTreeNext(tree,dimension);
    }
    /* A later zero dimension makes the tensor empty regardless of earlier
     * large dimensions. Detect zeros before multiplying, without saturation. */
    uint64_t count=empty?0U:1U;
    for(size_t i=0U;!empty && i<*rank;++i) {
        if(count>UINT64_MAX/dimensions[i]) return UMI_STATUS_CAPACITY_EXCEEDED;
        count*=dimensions[i];
    }
    if(count>UINT64_MAX/width) return UMI_STATUS_CAPACITY_EXCEEDED;
    int begin=UmiJsonTreeFirst(tree,offsets),end=UmiJsonTreeNext(tree,begin);
    status=tensor_unsigned(tree,begin,&range->begin);
    if(status==UMI_STATUS_OK) status=tensor_unsigned(tree,end,&range->end);
    if(status!=UMI_STATUS_OK) return status;
    if(range->end<range->begin || range->end>data_bytes ||
        range->end-range->begin!=count*width) return UMI_STATUS_PARSE_ERROR;
    *elements=count;return UMI_STATUS_OK;
}
static UmiStatus tensor_metadata(const UmiJsonTree *tree,int node,size_t *count,
    const UmiCancellationToken *cancel)
{
    if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT) return UMI_STATUS_PARSE_ERROR;
    *count=UmiJsonTreeCount(tree,node);
    int key=UmiJsonTreeFirst(tree,node);
    while(key>=0) {
        if(umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
        int value=UmiJsonTreeNext(tree,key);
        if(UmiJsonTreeKind(tree,value)!=UMI_LANGUAGE_RUNTIME_JSON_STRING) return UMI_STATUS_PARSE_ERROR;
        key=UmiJsonTreeNext(tree,value);
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiSafetensorsHeaderInspect(const void *prefix,size_t prefix_bytes,
    uint64_t file_bytes,const UmiCancellationToken *cancel,UmiSafetensorsHeaderInfo *out_info)
{
    if(prefix==NULL || out_info==NULL || prefix_bytes>file_bytes) return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_cancellation_token_is_requested(cancel)) return UMI_STATUS_CANCELLED;
    if(prefix_bytes<8U || file_bytes<10U) return UMI_STATUS_PARSE_ERROR;
    if(file_bytes>UMI_SAFETENSORS_MAX_FILE_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
/* Share length decoding with checked file inspection so both admission paths enforce the same bounds. The former implementation is retained for owner review. */
#if 0
    const unsigned char *bytes=prefix;uint64_t header=0U;
    for(size_t i=0U;i<8U;++i) header|=(uint64_t)bytes[i]<<(8U*i);
    if(header>UMI_SAFETENSORS_MAX_HEADER_BYTES) return UMI_STATUS_CAPACITY_EXCEEDED;
    if(header<2U || header>file_bytes-8U || header>prefix_bytes-8U || bytes[8]!='{')
        return UMI_STATUS_PARSE_ERROR;
#endif
    const unsigned char *bytes=prefix;uint64_t header;
    UmiStatus envelope=SafetensorsReadHeaderLength(bytes,&header);
    if(envelope!=UMI_STATUS_OK) return envelope;
    if(header>file_bytes-8U || header>prefix_bytes-8U || bytes[8]!='{') return UMI_STATUS_PARSE_ERROR;
    UmiJsonTreeLimits limits=UmiJsonTreeDefaultLimits();
    limits.bytes=UMI_SAFETENSORS_MAX_HEADER_BYTES;
    UmiJsonTree *tree=NULL;
    UmiStatus status=UmiJsonTreeCreate(bytes+8U,(size_t)header,&limits,cancel,&tree);
    if(status!=UMI_STATUS_OK) return status;
    TensorRange *ranges=NULL;
    UmiSafetensorsHeaderInfo candidate={0};
    candidate.header_bytes=header;candidate.data_bytes=file_bytes-8U-header;
    if(UmiJsonTreeKind(tree,0)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT) status=UMI_STATUS_PARSE_ERROR;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeValidateUniqueKeys(tree,0,cancel);
    const char *root=NULL;size_t root_bytes=0U;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeSourceSpan(tree,0,&root,&root_bytes);
    if(status==UMI_STATUS_OK)
        for(size_t i=root_bytes;i<(size_t)header;++i) {
            if((i&4095U)==0U && umi_cancellation_token_is_requested(cancel)) { status=UMI_STATUS_CANCELLED;break; }
            if(bytes[8U+i]!=' ') { status=UMI_STATUS_PARSE_ERROR;break; }
        }
    size_t members=UmiJsonTreeCount(tree,0);
    if(status==UMI_STATUS_OK && members>UMI_SAFETENSORS_MAX_TENSORS+1U)
        status=UMI_STATUS_CAPACITY_EXCEEDED;
    if(status==UMI_STATUS_OK && members!=0U) {
        ranges=calloc(members,sizeof(*ranges));
        if(ranges==NULL) status=UMI_STATUS_OUT_OF_MEMORY;
    }
    int key=UmiJsonTreeFirst(tree,0);
    while(status==UMI_STATUS_OK && key>=0) {
        if(umi_cancellation_token_is_requested(cancel)) { status=UMI_STATUS_CANCELLED;break; }
        char name[UMI_SAFETENSORS_MAX_NAME_BYTES+1U];
        status=UmiJsonTreeText(tree,key,name,sizeof(name));
        if(status!=UMI_STATUS_OK) break;
        int value=UmiJsonTreeNext(tree,key);
        if(strcmp(name,"__metadata__")==0) status=tensor_metadata(tree,value,&candidate.metadata_entries,cancel);
        else if(candidate.tensors==UMI_SAFETENSORS_MAX_TENSORS) status=UMI_STATUS_CAPACITY_EXCEEDED;
        else {
            uint64_t elements;size_t rank;
            status=tensor_record(tree,value,candidate.data_bytes,&ranges[candidate.tensors],&elements,&rank);
            if(status==UMI_STATUS_OK && elements>UINT64_MAX-candidate.elements)
                status=UMI_STATUS_CAPACITY_EXCEEDED;
            if(status==UMI_STATUS_OK) {
                ++candidate.tensors;candidate.elements+=elements;
                if(rank>candidate.maximum_rank) candidate.maximum_rank=rank;
            }
        }
        key=UmiJsonTreeNext(tree,value);
    }
    if(status==UMI_STATUS_OK && candidate.tensors>1U)
        qsort(ranges,candidate.tensors,sizeof(*ranges),tensor_range_order);
    uint64_t cursor=0U;
    for(size_t i=0U;status==UMI_STATUS_OK && i<candidate.tensors;++i) {
        if(umi_cancellation_token_is_requested(cancel)) { status=UMI_STATUS_CANCELLED;break; }
        if(ranges[i].begin!=cursor) status=UMI_STATUS_PARSE_ERROR;
        else cursor=ranges[i].end;
    }
    if(status==UMI_STATUS_OK && cursor!=candidate.data_bytes) status=UMI_STATUS_PARSE_ERROR;
    free(ranges);UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel)) status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK) *out_info=candidate;
    return status;
}

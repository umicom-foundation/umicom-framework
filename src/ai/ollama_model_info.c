/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/ollama_model_info.c
 *
 * PURPOSE:
 *   Decode bounded Ollama detail observations through shared JSON admission and own only selected copied metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ai/ollama_model_info.h"
#include "umicom/base/utf8.h"
#include "umicom/security/secrets.h"
#include "model_observation_json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct UmiOllamaModelInfo {
    UmiOllamaModelInfoSummary summary;
    char capabilities[UMI_OLLAMA_MODEL_CAPABILITY_LIMIT][UMI_OLLAMA_MODEL_CAPABILITY_CAPACITY];
};
static UmiStatus InfoLabel(const char *text,size_t capacity,size_t *out_length)
{
    if(text==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;while(length<capacity && text[length]!='\0')++length;
    if(length==capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    if(length==0U || !UmiUtf8Validate((const unsigned char *)text,length,NULL))return UMI_STATUS_INVALID_ARGUMENT;
    *out_length=length;return UMI_STATUS_OK;
}
static UmiStatus InfoDetails(const UmiJsonTree *tree,UmiOllamaModelInfoSummary *out)
{
    UmiStatus status=UmiAiObservationText(tree,0,"modified_at",0,out->modified_at,sizeof out->modified_at);
    if(status!=UMI_STATUS_OK)return status;
    int details=-1;status=UmiJsonTreeMember(tree,0,"details",&details);
    if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,details)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_PARSE_ERROR;
    status=UmiAiObservationText(tree,details,"parent_model",0,out->parent_model,sizeof out->parent_model);
    if(status==UMI_STATUS_OK)status=UmiAiObservationText(tree,details,"format",0,out->format,sizeof out->format);
    if(status==UMI_STATUS_OK)status=UmiAiObservationText(tree,details,"family",0,out->family,sizeof out->family);
    if(status==UMI_STATUS_OK)status=UmiAiObservationText(tree,details,"parameter_size",0,out->parameter_size,sizeof out->parameter_size);
    if(status==UMI_STATUS_OK)status=UmiAiObservationText(tree,details,"quantization_level",0,out->quantization,sizeof out->quantization);
    return status;
}
static UmiStatus InfoNumbers(const UmiJsonTree *tree,UmiOllamaModelInfoSummary *out)
{
    int model=-1;UmiStatus status=UmiJsonTreeMember(tree,0,"model_info",&model);
    if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,model)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT)return UMI_STATUS_PARSE_ERROR;
    status=UmiAiObservationText(tree,model,"general.architecture",0,out->architecture,sizeof out->architecture);
    if(status==UMI_STATUS_OK)status=UmiAiObservationUnsigned(tree,model,"general.parameter_count",
        &out->parameter_count,UMI_OLLAMA_MODEL_PARAMETER_COUNT,&out->known);
    if(status!=UMI_STATUS_OK || out->architecture[0]=='\0')return status;
    char key[160];int length=snprintf(key,sizeof key,"%s.context_length",out->architecture);
    if(length<0 || (size_t)length>=sizeof key)return UMI_STATUS_CAPACITY_EXCEEDED;
    status=UmiAiObservationUnsigned(tree,model,key,&out->context_length,UMI_OLLAMA_MODEL_CONTEXT_LENGTH,&out->known);
    if(status!=UMI_STATUS_OK)return status;
    length=snprintf(key,sizeof key,"%s.embedding_length",out->architecture);
    if(length<0 || (size_t)length>=sizeof key)return UMI_STATUS_CAPACITY_EXCEEDED;
    return UmiAiObservationUnsigned(tree,model,key,&out->embedding_length,UMI_OLLAMA_MODEL_EMBEDDING_LENGTH,&out->known);
}
static UmiStatus InfoCapabilities(const UmiJsonTree *tree,const UmiCancellationToken *cancel,UmiOllamaModelInfo *out)
{
    int list=-1;UmiStatus status=UmiJsonTreeMember(tree,0,"capabilities",&list);
    if(status==UMI_STATUS_NOT_FOUND)return UMI_STATUS_OK;
    if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,list)!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return UMI_STATUS_PARSE_ERROR;
    size_t count=UmiJsonTreeCount(tree,list);
    if(count>UMI_OLLAMA_MODEL_CAPABILITY_LIMIT)return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t index=0U;
    for(int node=UmiJsonTreeFirst(tree,list);node>=0;node=UmiJsonTreeNext(tree,node)){
        if(umi_cancellation_token_is_requested(cancel))return UMI_STATUS_CANCELLED;
        if(index>=count)return UMI_STATUS_INVALID_STATE;
        if(UmiJsonTreeKind(tree,node)!=UMI_LANGUAGE_RUNTIME_JSON_STRING)return UMI_STATUS_PARSE_ERROR;
        status=UmiJsonTreeText(tree,node,out->capabilities[index],sizeof out->capabilities[index]);
        if(status!=UMI_STATUS_OK)return status;
        if(out->capabilities[index][0]=='\0')return UMI_STATUS_PARSE_ERROR;
        /* A maximum of 64 declarations keeps the direct duplicate comparison
         * bounded without changing source order or adding another index owner. */
        for(size_t before=0U;before<index;++before)
            if(!strcmp(out->capabilities[before],out->capabilities[index]))return UMI_STATUS_ALREADY_EXISTS;
        ++index;
    }
    if(index!=count)return UMI_STATUS_INVALID_STATE;
    out->summary.capabilities_observed=true;out->summary.capability_count=count;return UMI_STATUS_OK;
}
void UmiOllamaModelInfoDestroy(UmiOllamaModelInfo *info)
{
    if(info!=NULL){umi_secret_clear(info,sizeof *info);free(info);}
}
UmiStatus UmiOllamaModelInfoCreate(const char *requested_model,const void *bytes,
    size_t length,const UmiCancellationToken *cancel,UmiOllamaModelInfo **out)
{
    if(out==NULL || *out!=NULL || bytes==NULL || length==0U)return UMI_STATUS_INVALID_ARGUMENT;
    size_t model_length=0U;UmiStatus status=InfoLabel(requested_model,UMI_AI_DISCOVERY_ID_CAPACITY,&model_length);
    if(status!=UMI_STATUS_OK)return status;
    if(length>UMI_OLLAMA_MODEL_INFO_MAX_BYTES)return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiJsonTreeLimits limits={UMI_OLLAMA_MODEL_INFO_MAX_BYTES,65536U,32U};
    UmiJsonTree *tree=NULL;UmiOllamaModelInfo *info=NULL;
    status=UmiJsonTreeCreate(bytes,length,&limits,cancel,&tree);if(status!=UMI_STATUS_OK)return status;
    if(UmiJsonTreeKind(tree,0)!=UMI_LANGUAGE_RUNTIME_JSON_OBJECT){status=UMI_STATUS_PARSE_ERROR;goto done;}
    status=UmiJsonTreeValidateUniqueKeys(tree,0,cancel);if(status!=UMI_STATUS_OK)goto done;
    int error=-1;status=UmiJsonTreeMember(tree,0,"error",&error);
    if(status==UMI_STATUS_OK){
        const char *span=NULL;size_t span_length=0U;
        if(UmiJsonTreeKind(tree,error)!=UMI_LANGUAGE_RUNTIME_JSON_STRING){status=UMI_STATUS_PARSE_ERROR;goto done;}
        status=UmiJsonTreeSourceSpan(tree,error,&span,&span_length);
        if(status==UMI_STATUS_OK)status=span_length>2U?UMI_STATUS_INVALID_STATE:UMI_STATUS_PARSE_ERROR;
        goto done;
    }
    if(status!=UMI_STATUS_NOT_FOUND)goto done;
    info=calloc(1U,sizeof *info);if(info==NULL){status=UMI_STATUS_OUT_OF_MEMORY;goto done;}
    memcpy(info->summary.requested_model,requested_model,model_length+1U);
    info->summary.response_bytes=length;
    status=InfoDetails(tree,&info->summary);
    if(status==UMI_STATUS_OK)status=InfoNumbers(tree,&info->summary);
    if(status==UMI_STATUS_OK)status=InfoCapabilities(tree,cancel,info);
    if(status==UMI_STATUS_OK){
        UmiSha256 hash;UmiSha256Init(&hash);status=UmiSha256Update(&hash,bytes,length);
        if(status==UMI_STATUS_OK)status=UmiSha256Final(&hash,info->summary.response_sha256);
    }
done:
    UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))status=UMI_STATUS_CANCELLED;
    if(status==UMI_STATUS_OK){*out=info;info=NULL;}
    UmiOllamaModelInfoDestroy(info);return status;
}
UmiStatus UmiOllamaModelInfoInspect(const UmiOllamaModelInfo *info,UmiOllamaModelInfoSummary *out)
{
    if(info==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    *out=info->summary;return UMI_STATUS_OK;
}
UmiStatus UmiOllamaModelInfoCapabilityAt(const UmiOllamaModelInfo *info,size_t index,char *out,size_t capacity)
{
    if(info==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(index>=info->summary.capability_count)return UMI_STATUS_NOT_FOUND;
    size_t length=strlen(info->capabilities[index]);
    if(length>=capacity)return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(out,info->capabilities[index],length+1U);return UMI_STATUS_OK;
}
UmiStatus UmiOllamaModelInfoDeclares(const UmiOllamaModelInfo *info,const char *capability,bool *out)
{
    if(info==NULL || out==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    size_t length=0U;UmiStatus status=InfoLabel(capability,UMI_OLLAMA_MODEL_CAPABILITY_CAPACITY,&length);
    if(status!=UMI_STATUS_OK)return status;
    if(!info->summary.capabilities_observed)return UMI_STATUS_NOT_FOUND;
    bool declared=false;
    for(size_t i=0U;i<info->summary.capability_count;++i)
        if(!strcmp(info->capabilities[i],capability)){declared=true;break;}
    *out=declared;return UMI_STATUS_OK;
}

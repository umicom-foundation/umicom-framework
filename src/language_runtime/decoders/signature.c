/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/signature.c
 *
 * PURPOSE:
 *   Decode active signature label/documentation/parameter.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/signature.h"
#include <string.h>
#include "umicom/language_runtime/signature_catalogue.h"
/*
 * Provide the language runtime decode signature operation used by this module and its
 * client applications.
 */
/* Owned signature decoding normalizes active choices and validates parameter spans before the legacy fixed-size projection. Oversized text now reports capacity instead of truncating; retain the former reader for review.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_decode_signature(const char*j,UmiLanguageRuntimeSignatureResult*out){UmiLanguageRuntimeJsonDocument d;int r,a,t,doc;int64_t as,ap;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;a=umi_language_runtime_json_object_get(&d,r,"signatures");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(a<0||!umi_language_runtime_json_array_count(&d,a))return UMI_STATUS_OK;as=umi_language_runtime_decoder_optional_int(&d,r,"activeSignature",0);ap=umi_language_runtime_decoder_optional_int(&d,r,"activeParameter",0);t=umi_language_runtime_json_array_at(&d,a,(size_t)(as>=0?as:0));/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t<0)t=umi_language_runtime_json_array_at(&d,a,0);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t<0)return UMI_STATUS_OK;umi_language_runtime_decoder_optional_string(&d,t,"label",out->label,sizeof(out->label));doc=umi_language_runtime_json_object_get(&d,t,"documentation");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(doc>=0){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[doc].type==UMI_LANGUAGE_RUNTIME_JSON_STRING)umi_language_runtime_json_string(&d,doc,out->documentation,sizeof(out->documentation));else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[doc].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){int v=umi_language_runtime_json_object_get(&d,doc,"value");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v>=0)umi_language_runtime_json_string(&d,v,out->documentation,sizeof(out->documentation));}}out->active_signature=as>=0?(uint32_t)as:0;out->active_parameter=ap>=0?(uint32_t)ap:0;out->available=out->label[0]!=0;return UMI_STATUS_OK;}
#endif
UmiStatus umi_language_runtime_decode_signature(const char *json,UmiLanguageRuntimeSignatureResult *out)
{
    if(json==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    /* Legacy dispatch already correlates the response. Keep that API while
     * projecting the selected complete overload only after strict decoding. */
    UmiJsonTree *tree=NULL;UmiJsonTreeLimits limits={1024U*1024U,131072U,16U};
    UmiStatus status=UmiJsonTreeCreate(json,strlen(json),&limits,NULL,&tree);
    int result=-1,error=-1;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeMember(tree,0,"result",&result);
    if(status==UMI_STATUS_NOT_FOUND) status=UMI_STATUS_PARSE_ERROR;
    if(status==UMI_STATUS_OK) {
        UmiStatus found=UmiJsonTreeMember(tree,0,"error",&error);
        if(found!=UMI_STATUS_NOT_FOUND) status=found==UMI_STATUS_OK?UMI_STATUS_PARSE_ERROR:found;
    }
    const char *span=NULL;size_t bytes=0U;UmiLanguageSignatureCatalogue *catalogue=NULL;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeSourceSpan(tree,result,&span,&bytes);
    if(status==UMI_STATUS_OK) status=UmiLanguageSignatureCatalogueCreate(span,bytes,NULL,&catalogue);
    UmiLanguageRuntimeSignatureResult candidate={0};
    if(status==UMI_STATUS_OK && UmiLanguageSignatureCatalogueCount(catalogue)!=0U) {
        UmiLanguageSignature signature;size_t active=UmiLanguageSignatureCatalogueActive(catalogue);
        status=UmiLanguageSignatureCatalogueAt(catalogue,active,&signature);
        if(status==UMI_STATUS_OK && (strlen(signature.label)>=sizeof(candidate.label) ||
            strlen(signature.documentation.text)>=sizeof(candidate.documentation))) status=UMI_STATUS_CAPACITY_EXCEEDED;
        if(status==UMI_STATUS_OK) {
            memcpy(candidate.label,signature.label,strlen(signature.label)+1U);
            memcpy(candidate.documentation,signature.documentation.text,strlen(signature.documentation.text)+1U);
            candidate.active_signature=(uint32_t)active;
            candidate.active_parameter=signature.active_parameter==SIZE_MAX?0U:(uint32_t)signature.active_parameter;
            candidate.available=signature.label[0]!='\0';
        }
    }
    UmiLanguageSignatureCatalogueDestroy(catalogue);UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK) *out=candidate;
    return status;
}

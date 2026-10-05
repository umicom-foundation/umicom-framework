/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/locations.c
 *
 * PURPOSE:
 *   Decode definition/reference locations and location links.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/locations.h"
#include <string.h>
#include "umicom/language_runtime/location_catalogue.h"
#include "umicom/language_runtime/json_tree.h"
#include <stdlib.h>
/* Complete owned decoding replaces silent truncation and skipped malformed
 * entries. Retain the former bounded projection for engineering review. */
#if 0
/* Provide the one operation used by this module and its client applications. */
static UmiStatus one(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeLocation*out){int u=umi_language_runtime_json_object_get(d,t,"uri"),r=umi_language_runtime_json_object_get(d,t,"range");UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(u>=0&&r>=0)return umi_language_runtime_decoder_location(d,t,out);u=umi_language_runtime_json_object_get(d,t,"targetUri");r=umi_language_runtime_json_object_get(d,t,"targetSelectionRange");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(u<0||r<0)return UMI_STATUS_PARSE_ERROR;memset(out,0,sizeof(*out));s=umi_language_runtime_json_string(d,u,out->uri,sizeof(out->uri));return s==UMI_STATUS_OK?umi_language_runtime_decoder_range(d,r,&out->range):s;}
/*
 * Provide the language runtime decode locations operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_locations(const char*j,UmiLanguageRuntimeLocationList*out){UmiLanguageRuntimeJsonDocument d;int r;size_t i,n;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[r].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){s=one(&d,r,&out->items[0]);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s==UMI_STATUS_OK)out->count=1;return s;}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[r].type!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return UMI_STATUS_PARSE_ERROR;n=umi_language_runtime_json_array_count(&d,r);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>UMI_LANGUAGE_RUNTIME_LOCATION_CAPACITY)n=UMI_LANGUAGE_RUNTIME_LOCATION_CAPACITY;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,r,i);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t>=0&&one(&d,t,&out->items[out->count])==UMI_STATUS_OK)out->count++;}return UMI_STATUS_OK;}

#endif
UmiStatus umi_language_runtime_decode_locations(const char *json,UmiLanguageRuntimeLocationList *out)
{
    if(json==NULL||out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    /* This compatibility API has no expected ID. Its transport caller must
     * correlate the response first. New clients use ReadResponse directly. */
    UmiJsonTree *tree=NULL;
    UmiJsonTreeLimits limits={1024U*1024U,131072U,16U};
    UmiStatus status=UmiJsonTreeCreate(json,strlen(json),&limits,NULL,&tree);
    int result=-1,error=-1;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeMember(tree,0,"result",&result);
    if(status==UMI_STATUS_NOT_FOUND) status=UMI_STATUS_PARSE_ERROR;
    if(status==UMI_STATUS_OK) {
        UmiStatus present=UmiJsonTreeMember(tree,0,"error",&error);
        if(present!=UMI_STATUS_NOT_FOUND) status=present==UMI_STATUS_OK?UMI_STATUS_PARSE_ERROR:present;
    }
    const char *value=NULL;size_t bytes=0U;char *wrapped=NULL;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeSourceSpan(tree,result,&value,&bytes);
    if(status==UMI_STATUS_OK&&UmiJsonTreeKind(tree,result)==UMI_LANGUAGE_RUNTIME_JSON_OBJECT) {
        int target=-1;
        UmiStatus found=UmiJsonTreeMember(tree,result,"targetUri",&target);
        if(found==UMI_STATUS_OK) {
            /* The older public API accepted a standalone link object. Keep
             * that convenience by wrapping it before the standard array reader. */
            wrapped=malloc(bytes+3U);
            if(wrapped==NULL) status=UMI_STATUS_OUT_OF_MEMORY;
            else {wrapped[0]='[';memcpy(wrapped+1U,value,bytes);wrapped[bytes+1U]=']';wrapped[bytes+2U]='\0';value=wrapped;bytes+=2U;}
        } else if(found!=UMI_STATUS_NOT_FOUND) status=found;
    }
    UmiLanguageLocationCatalogue *catalogue=NULL;
    if(status==UMI_STATUS_OK) status=UmiLanguageLocationCatalogueCreate(value,bytes,NULL,&catalogue);
    size_t count=UmiLanguageLocationCatalogueCount(catalogue);
    if(status==UMI_STATUS_OK&&count>UMI_LANGUAGE_RUNTIME_LOCATION_CAPACITY) status=UMI_STATUS_CAPACITY_EXCEEDED;
    UmiLanguageRuntimeLocationList *candidate=NULL;
    if(status==UMI_STATUS_OK) {candidate=calloc(1U,sizeof(*candidate));if(candidate==NULL) status=UMI_STATUS_OUT_OF_MEMORY;}
    for(size_t i=0U;status==UMI_STATUS_OK&&i<count;++i) {
        UmiLanguageSourceLocation location;
        status=UmiLanguageLocationCatalogueAt(catalogue,i,&location);
        if(status==UMI_STATUS_OK&&strlen(location.uri)>=sizeof(candidate->items[i].uri)) status=UMI_STATUS_CAPACITY_EXCEEDED;
        if(status==UMI_STATUS_OK) {
            memcpy(candidate->items[i].uri,location.uri,strlen(location.uri)+1U);
            candidate->items[i].range.start=(UmiLanguageRuntimePosition){(uint32_t)location.selection.start.line,(uint32_t)location.selection.start.utf16_column};
            candidate->items[i].range.end=(UmiLanguageRuntimePosition){(uint32_t)location.selection.end.line,(uint32_t)location.selection.end.utf16_column};
        }
    }
    if(status==UMI_STATUS_OK) {candidate->count=count;*out=*candidate;}
    free(candidate);free(wrapped);UmiLanguageLocationCatalogueDestroy(catalogue);UmiJsonTreeDestroy(tree);
    return status;
}

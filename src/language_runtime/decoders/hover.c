/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/hover.c
 *
 * PURPOSE:
 *   Decode string or MarkupContent hover payloads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/hover.h"
#include <string.h>
#include "umicom/language_runtime/hover_document.h"
#include "umicom/language_runtime/json_tree.h"
/*
 * Provide the language runtime decode hover operation used by this module and its client
 * applications.
 */
/* Owned hover decoding now retains array/code-block content and rejects malformed ranges instead of silently publishing an empty or partial result.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus umi_language_runtime_decode_hover(const char*j,UmiLanguageRuntimeHoverResult*out){UmiLanguageRuntimeJsonDocument d;int r,c,g;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;c=umi_language_runtime_json_object_get(&d,r,"contents");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c>=0){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[c].type==UMI_LANGUAGE_RUNTIME_JSON_STRING)s=umi_language_runtime_json_string(&d,c,out->contents,sizeof(out->contents));else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[c].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){int v=umi_language_runtime_json_object_get(&d,c,"value");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(v>=0)s=umi_language_runtime_json_string(&d,v,out->contents,sizeof(out->contents));}/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;}g=umi_language_runtime_json_object_get(&d,r,"range");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(g>=0&&umi_language_runtime_decoder_range(&d,g,&out->range)==UMI_STATUS_OK)out->has_range=1;return UMI_STATUS_OK;}
#endif

UmiStatus umi_language_runtime_decode_hover(const char *json,UmiLanguageRuntimeHoverResult *out)
{
    if(json==NULL || out==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out,0,sizeof(*out));
    /* The legacy API has no expected request ID. Its caller must correlate
     * the envelope first; new source tools use the owned ReadResponse API. */
    UmiJsonTree *tree=NULL;
    UmiJsonTreeLimits limits={1024U*1024U,16384U,16U};
    UmiStatus status=UmiJsonTreeCreate(json,strlen(json),&limits,NULL,&tree);
    int result=-1,error=-1;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeMember(tree,0,"result",&result);
    if(status==UMI_STATUS_NOT_FOUND) status=UMI_STATUS_PARSE_ERROR;
    if(status==UMI_STATUS_OK) {
        UmiStatus found=UmiJsonTreeMember(tree,0,"error",&error);
        if(found!=UMI_STATUS_NOT_FOUND) status=found==UMI_STATUS_OK?UMI_STATUS_PARSE_ERROR:found;
    }
    const char *value=NULL;size_t bytes=0U;
    if(status==UMI_STATUS_OK) status=UmiJsonTreeSourceSpan(tree,result,&value,&bytes);
    UmiLanguageHoverDocument *document=NULL;
    if(status==UMI_STATUS_OK) status=UmiLanguageHoverDocumentCreate(value,bytes,NULL,&document);
    if(status==UMI_STATUS_OK) status=UmiLanguageHoverDocumentText(document,&value,&bytes);
    UmiLanguageRuntimeHoverResult candidate={0};
    if(status==UMI_STATUS_OK && bytes>=sizeof(candidate.contents)) status=UMI_STATUS_CAPACITY_EXCEEDED;
    if(status==UMI_STATUS_OK) {
        memcpy(candidate.contents,value,bytes+1U);
        UmiEditorTextPosition start,end;
        UmiStatus range=UmiLanguageHoverDocumentRange(document,&start,&end);
        if(range==UMI_STATUS_OK) {
            candidate.range.start.line=(uint32_t)start.line;
            candidate.range.start.character=(uint32_t)start.utf16_column;
            candidate.range.end.line=(uint32_t)end.line;
            candidate.range.end.character=(uint32_t)end.utf16_column;
            candidate.has_range=1;
        } else if(range!=UMI_STATUS_NOT_FOUND) status=range;
    }
    UmiLanguageHoverDocumentDestroy(document);UmiJsonTreeDestroy(tree);
    if(status==UMI_STATUS_OK) *out=candidate;
    return status;
}

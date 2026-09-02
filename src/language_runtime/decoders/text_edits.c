/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/text_edits.c
 *
 * PURPOSE:
 *   Decode bounded LSP TextEdit arrays.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/text_edits.h"
#include <string.h>
/*
 * Provide the language runtime decode text edit array token operation used by this module
 * and its client applications.
 */
UmiStatus umi_language_runtime_decode_text_edit_array_token(const UmiLanguageRuntimeJsonDocument*d,int a,UmiLanguageRuntimeTextEditList*out){size_t i,n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!d||a<0||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));n=umi_language_runtime_json_array_count(d,a);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>UMI_LANGUAGE_RUNTIME_MAX_EDITS)n=UMI_LANGUAGE_RUNTIME_MAX_EDITS;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(d,a,i),r,x;UmiLanguageRuntimeTextEdit*e=&out->edits[out->count];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t<0)continue;r=umi_language_runtime_json_object_get(d,t,"range");x=umi_language_runtime_json_object_get(d,t,"newText");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||x<0||umi_language_runtime_decoder_range(d,r,&e->range)!=UMI_STATUS_OK||umi_language_runtime_json_string(d,x,e->new_text,sizeof(e->new_text))!=UMI_STATUS_OK)continue;out->count++;}return UMI_STATUS_OK;}
/*
 * Provide the language runtime decode text edits operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_text_edits(const char*j,UmiLanguageRuntimeTextEditList*out){UmiLanguageRuntimeJsonDocument d;int r;UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_json_parse(j,&d);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(r<0||umi_language_runtime_json_is_null(&d,r)){memset(out,0,sizeof(*out));return UMI_STATUS_OK;}return umi_language_runtime_decode_text_edit_array_token(&d,r,out);}

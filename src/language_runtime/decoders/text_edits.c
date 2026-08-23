/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/text_edits.c
 *
 * PURPOSE:
 *   Decode bounded LSP TextEdit arrays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/text_edits.h"
#include <string.h>
UmiStatus umi_language_runtime_decode_text_edit_array_token(const UmiLanguageRuntimeJsonDocument*d,int a,UmiLanguageRuntimeTextEditList*out){size_t i,n;if(!d||a<0||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));n=umi_language_runtime_json_array_count(d,a);if(n>UMI_LANGUAGE_RUNTIME_MAX_EDITS)n=UMI_LANGUAGE_RUNTIME_MAX_EDITS;for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(d,a,i),r,x;UmiLanguageRuntimeTextEdit*e=&out->edits[out->count];if(t<0)continue;r=umi_language_runtime_json_object_get(d,t,"range");x=umi_language_runtime_json_object_get(d,t,"newText");if(r<0||x<0||umi_language_runtime_decoder_range(d,r,&e->range)!=UMI_STATUS_OK||umi_language_runtime_json_string(d,x,e->new_text,sizeof(e->new_text))!=UMI_STATUS_OK)continue;out->count++;}return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_decode_text_edits(const char*j,UmiLanguageRuntimeTextEditList*out){UmiLanguageRuntimeJsonDocument d;int r;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r)){memset(out,0,sizeof(*out));return UMI_STATUS_OK;}return umi_language_runtime_decode_text_edit_array_token(&d,r,out);}

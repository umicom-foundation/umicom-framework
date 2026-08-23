/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/completion.c
 *
 * PURPOSE:
 *   Decode completion label/detail/insert/sort/kind fields.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/completion.h"
#include <string.h>
#include <stdio.h>
UmiStatus umi_language_runtime_decode_completion(const char*j,UmiLanguageRuntimeCompletionResult*out){UmiLanguageRuntimeJsonDocument d;int r,a;size_t i,n;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;if(d.tokens[r].type==UMI_LANGUAGE_RUNTIME_JSON_ARRAY)a=r;else if(d.tokens[r].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){a=umi_language_runtime_json_object_get(&d,r,"items");out->incomplete=umi_language_runtime_decoder_optional_bool(&d,r,"isIncomplete",0);}else return UMI_STATUS_PARSE_ERROR;if(a<0)return UMI_STATUS_PARSE_ERROR;n=umi_language_runtime_json_array_count(&d,a);if(n>UMI_LANGUAGE_RUNTIME_COMPLETION_CAPACITY)n=UMI_LANGUAGE_RUNTIME_COMPLETION_CAPACITY;for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,a,i);UmiLanguageRuntimeCompletionItem*x=&out->items[out->count];if(t<0||umi_language_runtime_decoder_optional_string(&d,t,"label",x->label,sizeof(x->label))!=UMI_STATUS_OK||!x->label[0])continue;umi_language_runtime_decoder_optional_string(&d,t,"detail",x->detail,sizeof(x->detail));umi_language_runtime_decoder_optional_string(&d,t,"insertText",x->insert_text,sizeof(x->insert_text));umi_language_runtime_decoder_optional_string(&d,t,"sortText",x->sort_text,sizeof(x->sort_text));x->kind=(int)umi_language_runtime_decoder_optional_int(&d,t,"kind",0);if(!x->insert_text[0]){size_t q=strlen(x->label);if(q>=sizeof(x->insert_text))q=sizeof(x->insert_text)-1;memcpy(x->insert_text,x->label,q);x->insert_text[q]=0;}out->count++;}return UMI_STATUS_OK;}

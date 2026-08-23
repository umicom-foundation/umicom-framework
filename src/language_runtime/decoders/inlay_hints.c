/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/inlay_hints.c
 *
 * PURPOSE:
 *   Decode common string-labelled LSP inlay hints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/inlay_hints.h"
#include <string.h>
UmiStatus umi_language_runtime_decode_inlay_hints(const char*j,UmiLanguageRuntimeInlayHintList*out){UmiLanguageRuntimeJsonDocument d;int r;size_t i,n;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;n=umi_language_runtime_json_array_count(&d,r);if(n>UMI_LANGUAGE_RUNTIME_HINT_CAPACITY)n=UMI_LANGUAGE_RUNTIME_HINT_CAPACITY;for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,r,i),p,l;UmiLanguageRuntimeInlayHint*x=&out->items[out->count];if(t<0)continue;p=umi_language_runtime_json_object_get(&d,t,"position");l=umi_language_runtime_json_object_get(&d,t,"label");if(p<0||l<0||umi_language_runtime_decoder_position(&d,p,&x->position)!=UMI_STATUS_OK||d.tokens[l].type!=UMI_LANGUAGE_RUNTIME_JSON_STRING||umi_language_runtime_json_string(&d,l,x->label,sizeof(x->label))!=UMI_STATUS_OK)continue;x->kind=(int)umi_language_runtime_decoder_optional_int(&d,t,"kind",0);x->padding_left=umi_language_runtime_decoder_optional_bool(&d,t,"paddingLeft",0);x->padding_right=umi_language_runtime_decoder_optional_bool(&d,t,"paddingRight",0);out->count++;}return UMI_STATUS_OK;}

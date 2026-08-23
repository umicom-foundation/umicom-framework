/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/signature.c
 *
 * PURPOSE:
 *   Decode active signature label/documentation/parameter.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/signature.h"
#include <string.h>
UmiStatus umi_language_runtime_decode_signature(const char*j,UmiLanguageRuntimeSignatureResult*out){UmiLanguageRuntimeJsonDocument d;int r,a,t,doc;int64_t as,ap;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;a=umi_language_runtime_json_object_get(&d,r,"signatures");if(a<0||!umi_language_runtime_json_array_count(&d,a))return UMI_STATUS_OK;as=umi_language_runtime_decoder_optional_int(&d,r,"activeSignature",0);ap=umi_language_runtime_decoder_optional_int(&d,r,"activeParameter",0);t=umi_language_runtime_json_array_at(&d,a,(size_t)(as>=0?as:0));if(t<0)t=umi_language_runtime_json_array_at(&d,a,0);if(t<0)return UMI_STATUS_OK;umi_language_runtime_decoder_optional_string(&d,t,"label",out->label,sizeof(out->label));doc=umi_language_runtime_json_object_get(&d,t,"documentation");if(doc>=0){if(d.tokens[doc].type==UMI_LANGUAGE_RUNTIME_JSON_STRING)umi_language_runtime_json_string(&d,doc,out->documentation,sizeof(out->documentation));else if(d.tokens[doc].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT){int v=umi_language_runtime_json_object_get(&d,doc,"value");if(v>=0)umi_language_runtime_json_string(&d,v,out->documentation,sizeof(out->documentation));}}out->active_signature=as>=0?(uint32_t)as:0;out->active_parameter=ap>=0?(uint32_t)ap:0;out->available=out->label[0]!=0;return UMI_STATUS_OK;}

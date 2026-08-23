/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/diagnostics.c
 *
 * PURPOSE:
 *   Decode diagnostic URI/version/range/severity/code/source/message.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/diagnostics.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_language_runtime_decode_diagnostics(const char*j,UmiLanguageRuntimeDiagnosticList*out){UmiLanguageRuntimeJsonDocument d;int p,u,a;size_t i,n;int64_t v;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;p=umi_language_runtime_decoder_params_token(&d);if(p<0)return UMI_STATUS_PARSE_ERROR;u=umi_language_runtime_json_object_get(&d,p,"uri");a=umi_language_runtime_json_object_get(&d,p,"diagnostics");if(u<0||a<0)return UMI_STATUS_PARSE_ERROR;s=umi_language_runtime_json_string(&d,u,out->uri,sizeof(out->uri));if(s!=UMI_STATUS_OK)return s;v=umi_language_runtime_decoder_optional_int(&d,p,"version",0);out->version=v>0?(uint64_t)v:0;n=umi_language_runtime_json_array_count(&d,a);if(n>UMI_LANGUAGE_RUNTIME_DIAGNOSTIC_CAPACITY)n=UMI_LANGUAGE_RUNTIME_DIAGNOSTIC_CAPACITY;for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,a,i),r,c;UmiLanguageRuntimeDiagnosticItem*x=&out->items[out->count];if(t<0)continue;r=umi_language_runtime_json_object_get(&d,t,"range");if(r<0||umi_language_runtime_decoder_range(&d,r,&x->range)!=UMI_STATUS_OK)continue;x->severity=(int)umi_language_runtime_decoder_optional_int(&d,t,"severity",3);umi_language_runtime_decoder_optional_string(&d,t,"source",x->source,sizeof(x->source));umi_language_runtime_decoder_optional_string(&d,t,"message",x->message,sizeof(x->message));c=umi_language_runtime_json_object_get(&d,t,"code");if(c>=0){if(d.tokens[c].type==UMI_LANGUAGE_RUNTIME_JSON_STRING)umi_language_runtime_json_string(&d,c,x->code,sizeof(x->code));else{int64_t q=0;if(umi_language_runtime_json_int64(&d,c,&q)==UMI_STATUS_OK)snprintf(x->code,sizeof(x->code),"%lld",(long long)q);}}out->count++;}return UMI_STATUS_OK;}

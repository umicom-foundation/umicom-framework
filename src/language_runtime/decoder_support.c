/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoder_support.c
 *
 * PURPOSE:
 *   Implement common LSP result/params/position/range/location decoding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoder_support.h"
#include <string.h>
int umi_language_runtime_decoder_result_token(const UmiLanguageRuntimeJsonDocument*d){return d&&d->token_count?umi_language_runtime_json_object_get(d,0,"result"):-1;}int umi_language_runtime_decoder_params_token(const UmiLanguageRuntimeJsonDocument*d){return d&&d->token_count?umi_language_runtime_json_object_get(d,0,"params"):-1;}
UmiStatus umi_language_runtime_decoder_position(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimePosition*out){int l,c;int64_t a=0,b=0;if(!d||!out||t<0)return UMI_STATUS_INVALID_ARGUMENT;l=umi_language_runtime_json_object_get(d,t,"line");c=umi_language_runtime_json_object_get(d,t,"character");if(l<0||c<0||umi_language_runtime_json_int64(d,l,&a)!=UMI_STATUS_OK||umi_language_runtime_json_int64(d,c,&b)!=UMI_STATUS_OK||a<0||b<0||a>UINT32_MAX||b>UINT32_MAX)return UMI_STATUS_PARSE_ERROR;out->line=(uint32_t)a;out->character=(uint32_t)b;return UMI_STATUS_OK;}
UmiStatus umi_language_runtime_decoder_range(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeRange*out){int a,b;UmiStatus s;if(!d||!out||t<0)return UMI_STATUS_INVALID_ARGUMENT;a=umi_language_runtime_json_object_get(d,t,"start");b=umi_language_runtime_json_object_get(d,t,"end");if(a<0||b<0)return UMI_STATUS_PARSE_ERROR;s=umi_language_runtime_decoder_position(d,a,&out->start);return s==UMI_STATUS_OK?umi_language_runtime_decoder_position(d,b,&out->end):s;}
UmiStatus umi_language_runtime_decoder_location(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeLocation*out){int u,r;UmiStatus s;if(!d||!out||t<0)return UMI_STATUS_INVALID_ARGUMENT;u=umi_language_runtime_json_object_get(d,t,"uri");r=umi_language_runtime_json_object_get(d,t,"range");if(u<0||r<0)return UMI_STATUS_PARSE_ERROR;memset(out,0,sizeof(*out));s=umi_language_runtime_json_string(d,u,out->uri,sizeof(out->uri));return s==UMI_STATUS_OK?umi_language_runtime_decoder_range(d,r,&out->range):s;}
UmiStatus umi_language_runtime_decoder_optional_string(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,char*out,size_t cap){int t;if(!out||!cap)return UMI_STATUS_INVALID_ARGUMENT;out[0]=0;t=umi_language_runtime_json_object_get(d,o,k);return t<0||umi_language_runtime_json_is_null(d,t)?UMI_STATUS_OK:umi_language_runtime_json_string(d,t,out,cap);}
int umi_language_runtime_decoder_optional_bool(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int def){int t=umi_language_runtime_json_object_get(d,o,k),v=def;if(t>=0)(void)umi_language_runtime_json_bool(d,t,&v);return v;}
int64_t umi_language_runtime_decoder_optional_int(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int64_t def){int t=umi_language_runtime_json_object_get(d,o,k);int64_t v=def;if(t>=0)(void)umi_language_runtime_json_int64(d,t,&v);return v;}

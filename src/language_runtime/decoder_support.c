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
/*
 * Provide the language runtime decoder result token operation used by this module and its
 * client applications.
 */
int umi_language_runtime_decoder_result_token(const UmiLanguageRuntimeJsonDocument*d){return d&&d->token_count?umi_language_runtime_json_object_get(d,0,"result"):-1;}int umi_language_runtime_decoder_params_token(const UmiLanguageRuntimeJsonDocument*d){return d&&d->token_count?umi_language_runtime_json_object_get(d,0,"params"):-1;}
/*
 * Provide the language runtime decoder position operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decoder_position(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimePosition*out){int l,c;int64_t a=0,b=0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!d||!out||t<0)return UMI_STATUS_INVALID_ARGUMENT;l=umi_language_runtime_json_object_get(d,t,"line");c=umi_language_runtime_json_object_get(d,t,"character");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(l<0||c<0||umi_language_runtime_json_int64(d,l,&a)!=UMI_STATUS_OK||umi_language_runtime_json_int64(d,c,&b)!=UMI_STATUS_OK||a<0||b<0||a>UINT32_MAX||b>UINT32_MAX)return UMI_STATUS_PARSE_ERROR;out->line=(uint32_t)a;out->character=(uint32_t)b;return UMI_STATUS_OK;}
/*
 * Provide the language runtime decoder range operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_decoder_range(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeRange*out){int a,b;UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!d||!out||t<0)return UMI_STATUS_INVALID_ARGUMENT;a=umi_language_runtime_json_object_get(d,t,"start");b=umi_language_runtime_json_object_get(d,t,"end");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(a<0||b<0)return UMI_STATUS_PARSE_ERROR;s=umi_language_runtime_decoder_position(d,a,&out->start);return s==UMI_STATUS_OK?umi_language_runtime_decoder_position(d,b,&out->end):s;}
/*
 * Provide the language runtime decoder location operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decoder_location(const UmiLanguageRuntimeJsonDocument*d,int t,UmiLanguageRuntimeLocation*out){int u,r;UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!d||!out||t<0)return UMI_STATUS_INVALID_ARGUMENT;u=umi_language_runtime_json_object_get(d,t,"uri");r=umi_language_runtime_json_object_get(d,t,"range");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(u<0||r<0)return UMI_STATUS_PARSE_ERROR;memset(out,0,sizeof(*out));s=umi_language_runtime_json_string(d,u,out->uri,sizeof(out->uri));return s==UMI_STATUS_OK?umi_language_runtime_decoder_range(d,r,&out->range):s;}
/*
 * Provide the language runtime decoder optional string operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_decoder_optional_string(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,char*out,size_t cap){int t;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!out||!cap)return UMI_STATUS_INVALID_ARGUMENT;out[0]=0;t=umi_language_runtime_json_object_get(d,o,k);return t<0||umi_language_runtime_json_is_null(d,t)?UMI_STATUS_OK:umi_language_runtime_json_string(d,t,out,cap);}
/*
 * Provide the language runtime decoder optional bool operation used by this module and its
 * client applications.
 */
int umi_language_runtime_decoder_optional_bool(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int def){int t=umi_language_runtime_json_object_get(d,o,k),v=def;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t>=0)(void)umi_language_runtime_json_bool(d,t,&v);return v;}
/*
 * Provide the language runtime decoder optional int operation used by this module and its
 * client applications.
 */
int64_t umi_language_runtime_decoder_optional_int(const UmiLanguageRuntimeJsonDocument*d,int o,const char*k,int64_t def){int t=umi_language_runtime_json_object_get(d,o,k);int64_t v=def;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t>=0)(void)umi_language_runtime_json_int64(d,t,&v);return v;}

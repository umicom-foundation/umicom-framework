/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/semantic_tokens.c
 *
 * PURPOSE:
 *   Decode relative semantic-token integer streams.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/semantic_tokens.h"
#include <string.h>
/*
 * Provide the language runtime decode semantic tokens operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_decode_semantic_tokens(const char*j,UmiLanguageRuntimeSemanticTokens*out){UmiLanguageRuntimeJsonDocument d;int r,a;size_t i,n;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;a=umi_language_runtime_json_object_get(&d,r,"data");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(a<0)return UMI_STATUS_PARSE_ERROR;n=umi_language_runtime_json_array_count(&d,a);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>UMI_LANGUAGE_RUNTIME_SEMANTIC_DATA_CAPACITY)return UMI_STATUS_CAPACITY_EXCEEDED;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,a,i);int64_t v=0;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t<0||umi_language_runtime_json_int64(&d,t,&v)!=UMI_STATUS_OK||v<0||v>UINT32_MAX)return UMI_STATUS_PARSE_ERROR;out->data[out->count++]=(uint32_t)v;}return UMI_STATUS_OK;}

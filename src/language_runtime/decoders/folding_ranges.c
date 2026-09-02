/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/folding_ranges.c
 *
 * PURPOSE:
 *   Decode line/character folding ranges.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/folding_ranges.h"
#include <string.h>
/*
 * Provide the language runtime decode folding ranges operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_folding_ranges(const char*j,UmiLanguageRuntimeFoldingRangeList*out){UmiLanguageRuntimeJsonDocument d;int r;size_t i,n;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;n=umi_language_runtime_json_array_count(&d,r);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>UMI_LANGUAGE_RUNTIME_FOLD_CAPACITY)n=UMI_LANGUAGE_RUNTIME_FOLD_CAPACITY;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,r,i);int64_t a,b,c,e;UmiLanguageRuntimeFoldingRange*x=&out->items[out->count];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t<0)continue;a=umi_language_runtime_decoder_optional_int(&d,t,"startLine",-1);b=umi_language_runtime_decoder_optional_int(&d,t,"endLine",-1);c=umi_language_runtime_decoder_optional_int(&d,t,"startCharacter",0);e=umi_language_runtime_decoder_optional_int(&d,t,"endCharacter",0);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(a<0||b<a||a>UINT32_MAX||b>UINT32_MAX||c<0||c>UINT32_MAX||e<0||e>UINT32_MAX)continue;x->start_line=(uint32_t)a;x->end_line=(uint32_t)b;x->start_character=(uint32_t)c;x->end_character=(uint32_t)e;umi_language_runtime_decoder_optional_string(&d,t,"kind",x->kind,sizeof(x->kind));out->count++;}return UMI_STATUS_OK;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/symbols.c
 *
 * PURPOSE:
 *   Decode flat document/workspace symbols for outline/navigation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/symbols.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_language_runtime_decode_symbols(const char*j,const char*du,UmiLanguageRuntimeSymbolList*out){UmiLanguageRuntimeJsonDocument d;int r;size_t i,n;UmiStatus s;if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;if(d.tokens[r].type!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return UMI_STATUS_PARSE_ERROR;n=umi_language_runtime_json_array_count(&d,r);if(n>UMI_LANGUAGE_RUNTIME_SYMBOL_CAPACITY)n=UMI_LANGUAGE_RUNTIME_SYMBOL_CAPACITY;for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,r,i),loc,rg;UmiLanguageRuntimeSymbolItem*x=&out->items[out->count];if(t<0||umi_language_runtime_decoder_optional_string(&d,t,"name",x->name,sizeof(x->name))!=UMI_STATUS_OK||!x->name[0])continue;umi_language_runtime_decoder_optional_string(&d,t,"containerName",x->container,sizeof(x->container));x->kind=(int)umi_language_runtime_decoder_optional_int(&d,t,"kind",0);loc=umi_language_runtime_json_object_get(&d,t,"location");if(loc>=0){UmiLanguageRuntimeLocation l;if(umi_language_runtime_decoder_location(&d,loc,&l)==UMI_STATUS_OK){snprintf(x->uri,sizeof(x->uri),"%s",l.uri);x->range=l.range;}}else{rg=umi_language_runtime_json_object_get(&d,t,"selectionRange");if(rg<0)rg=umi_language_runtime_json_object_get(&d,t,"range");if(rg>=0)(void)umi_language_runtime_decoder_range(&d,rg,&x->range);if(du)snprintf(x->uri,sizeof(x->uri),"%s",du);}out->count++;}return UMI_STATUS_OK;}

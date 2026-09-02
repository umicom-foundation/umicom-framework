/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/code_actions.c
 *
 * PURPOSE:
 *   Decode quick-fix/refactor title/kind/command/preference fields.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/code_actions.h"
#include <string.h>
/*
 * Provide the language runtime decode code actions operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_decode_code_actions(const char*j,UmiLanguageRuntimeCodeActionList*out){UmiLanguageRuntimeJsonDocument d;int r;size_t i,n;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(r<0||umi_language_runtime_json_is_null(&d,r))return UMI_STATUS_OK;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[r].type!=UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return UMI_STATUS_PARSE_ERROR;n=umi_language_runtime_json_array_count(&d,r);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n>UMI_LANGUAGE_RUNTIME_CODE_ACTION_CAPACITY)n=UMI_LANGUAGE_RUNTIME_CODE_ACTION_CAPACITY;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<n;i++){int t=umi_language_runtime_json_array_at(&d,r,i),c;UmiLanguageRuntimeCodeActionItem*x=&out->items[out->count];/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(t<0||umi_language_runtime_decoder_optional_string(&d,t,"title",x->title,sizeof(x->title))!=UMI_STATUS_OK||!x->title[0])continue;umi_language_runtime_decoder_optional_string(&d,t,"kind",x->kind,sizeof(x->kind));x->preferred=umi_language_runtime_decoder_optional_bool(&d,t,"isPreferred",0);c=umi_language_runtime_json_object_get(&d,t,"command");/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c>=0){/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[c].type==UMI_LANGUAGE_RUNTIME_JSON_STRING)umi_language_runtime_json_string(&d,c,x->command,sizeof(x->command));else /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(d.tokens[c].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT)umi_language_runtime_decoder_optional_string(&d,c,"command",x->command,sizeof(x->command));}out->count++;}return UMI_STATUS_OK;}

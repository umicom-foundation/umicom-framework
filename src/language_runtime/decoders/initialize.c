/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/decoders/initialize.c
 *
 * PURPOSE:
 *   Decode common server capability values from initialize result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/decoders/initialize.h"
#include <string.h>
/* Provide the on operation used by this module and its client applications. */
static int on(const UmiLanguageRuntimeJsonDocument*d,int c,const char*k){int t=umi_language_runtime_json_object_get(d,c,k),b=0;int64_t n=0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(t<0||umi_language_runtime_json_is_null(d,t))return 0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(d->tokens[t].type==UMI_LANGUAGE_RUNTIME_JSON_OBJECT||d->tokens[t].type==UMI_LANGUAGE_RUNTIME_JSON_ARRAY)return 1;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_json_bool(d,t,&b)==UMI_STATUS_OK)return b;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_language_runtime_json_int64(d,t,&n)==UMI_STATUS_OK)return n!=0;return 0;}
/*
 * Initialise language runtime decode from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_runtime_decode_initialize(const char*j,UmiLanguageRuntimeInitializeResult*out){UmiLanguageRuntimeJsonDocument d;int r,c;UmiStatus s;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(!j||!out)return UMI_STATUS_INVALID_ARGUMENT;memset(out,0,sizeof(*out));s=umi_language_runtime_json_parse(j,&d);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(s!=UMI_STATUS_OK)return s;r=umi_language_runtime_decoder_result_token(&d);c=r>=0?umi_language_runtime_json_object_get(&d,r,"capabilities"):-1;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(c<0)return UMI_STATUS_PARSE_ERROR;out->text_document_sync=on(&d,c,"textDocumentSync");out->completion=on(&d,c,"completionProvider");out->hover=on(&d,c,"hoverProvider");out->signature_help=on(&d,c,"signatureHelpProvider");out->definition=on(&d,c,"definitionProvider");out->references=on(&d,c,"referencesProvider");out->document_symbols=on(&d,c,"documentSymbolProvider");out->workspace_symbols=on(&d,c,"workspaceSymbolProvider");out->code_actions=on(&d,c,"codeActionProvider");out->formatting=on(&d,c,"documentFormattingProvider");out->rename=on(&d,c,"renameProvider");out->semantic_tokens=on(&d,c,"semanticTokensProvider");out->inlay_hints=on(&d,c,"inlayHintProvider");out->folding_ranges=on(&d,c,"foldingRangeProvider");return UMI_STATUS_OK;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/initialize.c
 *
 * PURPOSE:
 *   Advertise completion, navigation, refactoring, semantic tokens, hints and folding capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/initialize.h"
#include "umicom/language_runtime/json_writer.h"
/*
 * Initialise language runtime request from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_language_runtime_request_initialize(UmiLanguageRuntimeServer*s,const char*root,uint64_t*out){char p[8192];UmiLanguageRuntimeJsonWriter w;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!root||!*root)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"processId\":null,\"clientInfo\":{\"name\":\"Umicom Framework\",\"version\":\"0.9.0\"},\"rootUri\":");umi_language_runtime_json_writer_string(&w,root);umi_language_runtime_json_writer_raw(&w,",\"capabilities\":{\"workspace\":{\"workspaceFolders\":true,\"symbol\":{}},\"textDocument\":{\"synchronization\":{\"didSave\":true},\"completion\":{\"completionItem\":{\"snippetSupport\":true}},\"hover\":{},\"signatureHelp\":{},\"definition\":{},\"references\":{},\"documentSymbol\":{},\"codeAction\":{},\"formatting\":{},\"rename\":{\"prepareSupport\":true},\"semanticTokens\":{\"requests\":{\"full\":true},\"tokenTypes\":[],\"tokenModifiers\":[],\"formats\":[\"relative\"]},\"inlayHint\":{},\"foldingRange\":{\"lineFoldingOnly\":true}}},\"workspaceFolders\":[{\"uri\":");umi_language_runtime_json_writer_string(&w,root);umi_language_runtime_json_writer_raw(&w,",\"name\":\"workspace\"}]}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"initialize",p,"",out):w.status;}

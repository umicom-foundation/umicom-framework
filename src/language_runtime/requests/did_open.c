/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/did_open.c
 *
 * PURPOSE:
 *   Implement textDocument/didOpen full-text synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/did_open.h"
/*
 * Provide the language runtime request did open operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_did_open(UmiLanguageRuntimeServer*s,const char*u,const char*lang,uint64_t ver,const char*text){char p[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];UmiLanguageRuntimeJsonWriter w;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!u||!lang||!text||!ver)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,",\"languageId\":");umi_language_runtime_json_writer_string(&w,lang);umi_language_runtime_json_writer_raw(&w,",\"version\":");umi_language_runtime_json_writer_uint64(&w,ver);umi_language_runtime_json_writer_raw(&w,",\"text\":");umi_language_runtime_json_writer_string(&w,text);umi_language_runtime_json_writer_raw(&w,"}}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_notification(s,"textDocument/didOpen",p):w.status;}

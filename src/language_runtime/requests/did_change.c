/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/did_change.c
 *
 * PURPOSE:
 *   Implement textDocument/didChange full-text synchronization.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/did_change.h"
UmiStatus umi_language_runtime_request_did_change(UmiLanguageRuntimeServer*s,const char*u,uint64_t ver,const char*text){char p[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];UmiLanguageRuntimeJsonWriter w;if(!s||!u||!text||!ver)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,",\"version\":");umi_language_runtime_json_writer_uint64(&w,ver);umi_language_runtime_json_writer_raw(&w,"},\"contentChanges\":[{\"text\":");umi_language_runtime_json_writer_string(&w,text);umi_language_runtime_json_writer_raw(&w,"}]}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_notification(s,"textDocument/didChange",p):w.status;}

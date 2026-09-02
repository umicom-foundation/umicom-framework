/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/did_save.c
 *
 * PURPOSE:
 *   Implement textDocument/didSave full-text synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/did_save.h"
/*
 * Write language runtime request did in its stable representation and report capacity or
 * input failures to the caller.
 */
UmiStatus umi_language_runtime_request_did_save(UmiLanguageRuntimeServer*s,const char*u,const char*text){char p[UMI_LANGUAGE_RUNTIME_JSON_CAPACITY];UmiLanguageRuntimeJsonWriter w;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!u)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"}");/* Preserve the original failure result so the caller can respond to the correct cause. */ if(text){umi_language_runtime_json_writer_raw(&w,",\"text\":");umi_language_runtime_json_writer_string(&w,text);}umi_language_runtime_json_writer_raw(&w,"}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_notification(s,"textDocument/didSave",p):w.status;}

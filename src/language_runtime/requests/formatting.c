/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/formatting.c
 *
 * PURPOSE:
 *   Implement textDocument/formatting.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/formatting.h"
/*
 * Provide the language runtime request formatting operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_formatting(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t tab,int spaces,uint64_t*out){char p[2048];UmiLanguageRuntimeJsonWriter w;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!d||!u||!tab)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"},\"options\":{\"tabSize\":");umi_language_runtime_json_writer_uint64(&w,tab);umi_language_runtime_json_writer_raw(&w,",\"insertSpaces\":");umi_language_runtime_json_writer_bool(&w,spaces);umi_language_runtime_json_writer_raw(&w,"}}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/formatting",p,d,out):w.status;}

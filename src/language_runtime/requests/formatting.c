/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/formatting.c
 *
 * PURPOSE:
 *   Implement textDocument/formatting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/formatting.h"
UmiStatus umi_language_runtime_request_formatting(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t tab,int spaces,uint64_t*out){char p[2048];UmiLanguageRuntimeJsonWriter w;if(!s||!d||!u||!tab)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"},\"options\":{\"tabSize\":");umi_language_runtime_json_writer_uint64(&w,tab);umi_language_runtime_json_writer_raw(&w,",\"insertSpaces\":");umi_language_runtime_json_writer_bool(&w,spaces);umi_language_runtime_json_writer_raw(&w,"}}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/formatting",p,d,out):w.status;}

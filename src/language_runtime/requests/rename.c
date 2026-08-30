/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/rename.c
 *
 * PURPOSE:
 *   Implement textDocument/rename.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/rename.h"
UmiStatus umi_language_runtime_request_rename(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t l,uint32_t c,const char*n,uint64_t*out){char p[4096];UmiLanguageRuntimeJsonWriter w;if(!s||!d||!u||!n||!*n)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"},\"position\":{\"line\":");umi_language_runtime_json_writer_uint64(&w,l);umi_language_runtime_json_writer_raw(&w,",\"character\":");umi_language_runtime_json_writer_uint64(&w,c);umi_language_runtime_json_writer_raw(&w,"},\"newName\":");umi_language_runtime_json_writer_string(&w,n);umi_language_runtime_json_writer_raw(&w,"}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/rename",p,d,out):w.status;}

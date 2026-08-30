/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/references.c
 *
 * PURPOSE:
 *   Implement textDocument/references.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/references.h"
UmiStatus umi_language_runtime_request_references(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t l,uint32_t c,int inc,uint64_t*out){char p[2048];UmiLanguageRuntimeJsonWriter w;if(!s||!d||!u)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,p,sizeof(p));umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"},\"position\":{\"line\":");umi_language_runtime_json_writer_uint64(&w,l);umi_language_runtime_json_writer_raw(&w,",\"character\":");umi_language_runtime_json_writer_uint64(&w,c);umi_language_runtime_json_writer_raw(&w,"},\"context\":{\"includeDeclaration\":");umi_language_runtime_json_writer_bool(&w,inc);umi_language_runtime_json_writer_raw(&w,"}}");return w.status==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/references",p,d,out):w.status;}

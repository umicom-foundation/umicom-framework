/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/request_support.c
 *
 * PURPOSE:
 *   Implement common textDocument/position/range/workspace JSON parameters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/request_support.h"
/* Provide the pos operation used by this module and its client applications. */
static void pos(UmiLanguageRuntimeJsonWriter*w,uint32_t l,uint32_t c){umi_language_runtime_json_writer_raw(w,"{\"line\":");umi_language_runtime_json_writer_uint64(w,l);umi_language_runtime_json_writer_raw(w,",\"character\":");umi_language_runtime_json_writer_uint64(w,c);umi_language_runtime_json_writer_raw(w,"}");}
/*
 * Provide the language runtime params position operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_params_position(const char*u,uint32_t l,uint32_t c,char*out,size_t cap){UmiLanguageRuntimeJsonWriter w;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!u||!*u||!out||!cap)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,out,cap);umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"},\"position\":");pos(&w,l,c);umi_language_runtime_json_writer_raw(&w,"}");return w.status;}
/*
 * Provide the language runtime params document operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_params_document(const char*u,char*out,size_t cap){UmiLanguageRuntimeJsonWriter w;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!u||!*u||!out||!cap)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,out,cap);umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"}}");return w.status;}
/*
 * Provide the language runtime params range operation used by this module and its client
 * applications.
 */
UmiStatus umi_language_runtime_params_range(const char*u,const UmiLanguageRuntimeRange*r,char*out,size_t cap){UmiLanguageRuntimeJsonWriter w;UmiStatus s;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!u||!r||!out||!cap)return UMI_STATUS_INVALID_ARGUMENT;s=umi_language_runtime_range_validate(r);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(s!=UMI_STATUS_OK)return s;umi_language_runtime_json_writer_init(&w,out,cap);umi_language_runtime_json_writer_raw(&w,"{\"textDocument\":{\"uri\":");umi_language_runtime_json_writer_string(&w,u);umi_language_runtime_json_writer_raw(&w,"},\"range\":{\"start\":");pos(&w,r->start.line,r->start.character);umi_language_runtime_json_writer_raw(&w,",\"end\":");pos(&w,r->end.line,r->end.character);umi_language_runtime_json_writer_raw(&w,"}}");return w.status;}
/*
 * Provide the language runtime params workspace query operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_params_workspace_query(const char*q,char*out,size_t cap){UmiLanguageRuntimeJsonWriter w;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!q||!out||!cap)return UMI_STATUS_INVALID_ARGUMENT;umi_language_runtime_json_writer_init(&w,out,cap);umi_language_runtime_json_writer_raw(&w,"{\"query\":");umi_language_runtime_json_writer_string(&w,q);umi_language_runtime_json_writer_raw(&w,"}");return w.status;}

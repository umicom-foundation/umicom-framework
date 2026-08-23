/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/completion.c
 *
 * PURPOSE:
 *   Implement the LSP textDocument/completion request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/completion.h"
UmiStatus umi_language_runtime_request_completion(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t l,uint32_t c,uint64_t*out){char p[2048];UmiStatus q;if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_position(u,l,c,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/completion",p,d,out):q;}

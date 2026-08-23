/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/hover.c
 *
 * PURPOSE:
 *   Implement the LSP textDocument/hover request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/hover.h"
UmiStatus umi_language_runtime_request_hover(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t l,uint32_t c,uint64_t*out){char p[2048];UmiStatus q;if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_position(u,l,c,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/hover",p,d,out):q;}

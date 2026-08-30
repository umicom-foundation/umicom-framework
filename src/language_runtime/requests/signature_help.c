/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/signature_help.c
 *
 * PURPOSE:
 *   Implement the LSP textDocument/signatureHelp request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/signature_help.h"
UmiStatus umi_language_runtime_request_signature_help(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint32_t l,uint32_t c,uint64_t*out){char p[2048];UmiStatus q;if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_position(u,l,c,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/signatureHelp",p,d,out):q;}

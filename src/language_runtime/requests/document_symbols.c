/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/document_symbols.c
 *
 * PURPOSE:
 *   Implement the LSP textDocument/documentSymbol request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/document_symbols.h"
UmiStatus umi_language_runtime_request_document_symbols(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint64_t*out){char p[2048];UmiStatus q;if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_document(u,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/documentSymbol",p,d,out):q;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/folding_ranges.c
 *
 * PURPOSE:
 *   Implement the LSP textDocument/foldingRange request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/folding_ranges.h"
/*
 * Provide the language runtime request folding ranges operation used by this module and
 * its client applications.
 */
UmiStatus umi_language_runtime_request_folding_ranges(UmiLanguageRuntimeServer*s,const char*d,const char*u,uint64_t*out){char p[2048];UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_document(u,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/foldingRange",p,d,out):q;}

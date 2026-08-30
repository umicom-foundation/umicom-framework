/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/inlay_hints.c
 *
 * PURPOSE:
 *   Implement textDocument/inlayHint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/inlay_hints.h"
UmiStatus umi_language_runtime_request_inlay_hints(UmiLanguageRuntimeServer*s,const char*d,const char*u,const UmiLanguageRuntimeRange*r,uint64_t*out){char p[4096];UmiStatus q;if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_range(u,r,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"textDocument/inlayHint",p,d,out):q;}

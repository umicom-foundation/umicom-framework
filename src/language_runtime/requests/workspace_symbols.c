/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/workspace_symbols.c
 *
 * PURPOSE:
 *   Implement workspace/symbol.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/workspace_symbols.h"
UmiStatus umi_language_runtime_request_workspace_symbols(UmiLanguageRuntimeServer*s,const char*q,uint64_t*out){char p[2048];UmiStatus x;if(!s||!q)return UMI_STATUS_INVALID_ARGUMENT;x=umi_language_runtime_params_workspace_query(q,p,sizeof(p));return x==UMI_STATUS_OK?umi_language_runtime_server_send_request(s,"workspace/symbol",p,"",out):x;}

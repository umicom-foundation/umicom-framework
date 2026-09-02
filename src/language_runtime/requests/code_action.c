/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/code_action.c
 *
 * PURPOSE:
 *   Implement textDocument/codeAction with explicit bounded range/context.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/code_action.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the language runtime request code action operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_code_action(UmiLanguageRuntimeServer*s,const char*d,const char*u,const UmiLanguageRuntimeRange*r,uint64_t*out){char base[4096],p[4608];UmiStatus q;size_t n;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s||!d)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_range(u,r,base,sizeof(base));/* Preserve the original failure result so the caller can respond to the correct cause. */ if(q!=UMI_STATUS_OK)return q;n=strlen(base);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(n<2)return UMI_STATUS_PARSE_ERROR;base[n-1]=0;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(snprintf(p,sizeof(p),"%s,\"context\":{\"diagnostics\":[]}}",base)<0)return UMI_STATUS_INTERNAL_ERROR;return umi_language_runtime_server_send_request(s,"textDocument/codeAction",p,d,out);}

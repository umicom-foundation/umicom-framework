/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/did_close.c
 *
 * PURPOSE:
 *   Implement textDocument/didClose full-text synchronization.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/did_close.h"
/*
 * Provide the language runtime request did close operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_did_close(UmiLanguageRuntimeServer*s,const char*u){char p[2048];UmiStatus q;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!s)return UMI_STATUS_INVALID_ARGUMENT;q=umi_language_runtime_params_document(u,p,sizeof(p));return q==UMI_STATUS_OK?umi_language_runtime_server_send_notification(s,"textDocument/didClose",p):q;}

/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/requests/shutdown.c
 *
 * PURPOSE:
 *   Implement LSP shutdown lifecycle message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/requests/shutdown.h"
/*
 * Provide the language runtime request shutdown operation used by this module and its
 * client applications.
 */
UmiStatus umi_language_runtime_request_shutdown(UmiLanguageRuntimeServer*s,uint64_t*out){return s?umi_language_runtime_server_send_request(s,"shutdown",NULL,"",out):UMI_STATUS_INVALID_ARGUMENT;}
